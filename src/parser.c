#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "machine.h"
#include "lexer.h"

typedef struct {
    char *str;
    int val;
} PiParserDef;

typedef struct {
    int *where;
    char *what;
} PiRevisitDef;

PiParserDef *p_defs;
int p_defs_n = 0;
int p_memory_loc = 0;

PiRevisitDef *p_revisits;
int p_revisits_n = 0;

// TODO: move it into PiParser
void P_AddDef(char *str, int val) {
    p_defs_n++;
    p_defs = realloc(p_defs, p_defs_n * sizeof(*p_defs));

    p_defs[p_defs_n - 1].str = strdup(str);
    p_defs[p_defs_n - 1].val = val;
}

void P_AddRevisit(int *where, char *what) {
    p_revisits_n++;
    p_revisits = realloc(p_revisits, p_revisits_n * sizeof(*p_revisits));

    p_revisits[p_revisits_n - 1].where = where;
    p_revisits[p_revisits_n - 1].what = strdup(what);
}

void P_ParserCreate(PiParser *par, PiLexer *lexer, char *out) {
    par->lexer      = lexer;
    par->buf[0]     = L_TokenNew(0, 0, 0, NULL);
    par->buf[1]     = L_TokenNew(0, 0, 0, NULL);
    par->loc        = 2;
    par->out        = out;
    par->out_start  = out;

    P_AddDef("read_int", 0);
    P_AddDef("print_int", 1);
    P_AddDef("print_str", 2);
    P_AddDef("println_str", 3);
    P_AddDef("println_int", 4);
}

void P_ParserDelete(PiParser *par) {
    L_TokenDelete(par->buf);
    L_TokenDelete(par->buf + 1);
}

PiToken *P_ParserReadToken(PiParser *par) {
    if (par->loc == 2) {
        L_TokenDelete(&par->buf[0]);
        par->buf[0] = par->buf[1];
        par->buf[1] = L_LexerReadToken(par->lexer);

        return &par->buf[1];
    }

    return &par->buf[par->loc++];
}

void P_ParserStepBack(PiParser *par) {
    par->loc = 1;
}

void P_ParserGetStatement(PiParser *par) {
    PiToken *tok = P_ParserReadToken(par);

    switch (tok->type) {
    case TOK_RETURN:
        tok = P_ParserReadToken(par);
        if (tok->type != TOK_SEMICOLON)
            P_ParserNewError(par, "Semicolon expected");

        P_ParserWrite8(par, OP_RETURN);
        return;

    case TOK_IF:
        {
            tok = P_ParserReadToken(par);
            if (tok->type != TOK_LBRACE) { P_ParserNewError(par, "'(' expected"); return; }

            P_ParserGetMathExpr(par, 0);

            P_ParserWrite8(par, OP_JMPZ);
            int *jmpz_ptr = (int*)par->out;
            P_ParserWrite32(par, 0);

            tok = P_ParserReadToken(par);
            if (tok->type != TOK_RBRACE) { P_ParserNewError(par, "')' expected"); return; }

            P_ParserGetStatement(par);

            tok = P_ParserReadToken(par);
            if (tok->type == TOK_ELSE) {
                P_ParserWrite8(par, OP_JMP);
                int *jmpz_ptr2 = (int*)par->out;
                P_ParserWrite32(par, 0);

                *jmpz_ptr = (par->out - par->out_start);
                P_ParserGetStatement(par);
                *jmpz_ptr2 = (par->out - par->out_start);
            }
            else {
                P_ParserStepBack(par);
                *jmpz_ptr = (par->out - par->out_start);
            }
            return;
        }

    case TOK_LBRACKET:
        tok = P_ParserReadToken(par);
        if (tok->type == TOK_GLOB) {
            while (1) {
                tok = P_ParserReadToken(par);
                if (tok->type != TOK_IDENT) {
                    P_ParserNewError(par, "Ident expected");
                    return;
                }
                P_AddDef(tok->str, p_memory_loc);
                p_memory_loc += 4;

                tok = P_ParserReadToken(par);
                if (tok->type == TOK_COMMA)
                    continue;
                else if (tok->type == TOK_SEMICOLON)
                    break;
                else {
                    P_ParserNewError(par, "';' expected");
                    return;
                }
            }
        }
        else P_ParserStepBack(par);

        while (1) {
            tok = P_ParserReadToken(par);
            if (tok->type == TOK_RBRACKET)
                break;

            P_ParserStepBack(par);
            P_ParserGetStatement(par);
        }
        return;

    case TOK_WHILE:
        {
            int cond_addr = (par->out - par->out_start);
            tok = P_ParserReadToken(par);
            if (tok->type != TOK_LBRACE) {
                P_ParserNewError(par, "'(' expected");
                return;
            }

            P_ParserGetMathExpr(par, 0);

            P_ParserWrite8(par, OP_JMPZ);
            int *jmpz_ptr = (char*)par->out;
            P_ParserWrite32(par, 0);

            tok = P_ParserReadToken(par);
            if (tok->type != TOK_RBRACE) {
                P_ParserNewError(par, "')' expected");
                return;
            }

            P_ParserGetStatement(par);
            P_ParserWrite8(par, OP_JMP);
            P_ParserWrite32(par, cond_addr);

            *jmpz_ptr = (par->out - par->out_start);
            return;
        }
    }

    P_ParserStepBack(par);
    P_ParserGetMathExpr(par, 0);
    tok = P_ParserReadToken(par);
    if (tok->type != TOK_SEMICOLON) {
        P_ParserNewError(par, "';' expected");
        return;
    }

    return;
}

void P_ParserRevisit(PiParser *par) {
    for (int i = 0; i < p_revisits_n; i++) {
        *p_revisits[i].where = (par->out - par->out_start);

        for (char *c = p_revisits[i].what; *c; c++) P_ParserWrite8(par, *c);
        P_ParserWrite8(par, 0);
    }
}

void P_ParserGetJustPrimary(PiParser *par) {
    PiToken *tok = P_ParserReadToken(par);

    switch (tok->type) {
    case TOK_NUMBER:
        P_ParserWrite8(par, OP_CONST);
        P_ParserWrite32(par, atoi(tok->str));
        return;
    case TOK_STRLITERAL:
        P_ParserWrite8(par, OP_CONST);
        P_AddRevisit(par->out, tok->str);
        P_ParserWrite32(par, 0);
        return;

    case TOK_IDENT:
        for (int i = 0; i < p_defs_n; i++) {
            if (!strcmp(tok->str, p_defs[i].str)) {
                P_ParserWrite8(par, OP_CONST);
                P_ParserWrite32(par, p_defs[i].val);
                return;
            }
        }
        P_ParserWrite8(par, OP_CONST);
        P_ParserWrite32(par, -1); // TODO: Error message
        return;

    case TOK_LBRACE:
        P_ParserGetMathExpr(par, 0);
        tok = P_ParserReadToken(par);

        if (tok->type == TOK_RBRACE)
            return;
        else {
            P_ParserStepBack(par);
            P_ParserNewError(par, "')' expected");
            return;
        }
    case TOK_DEREF:
        P_ParserGetJustPrimary(par);
        P_ParserWrite8(par, OP_LOAD);
        return;
    }

    P_ParserNewError(par, "Unexpected token");
    P_ParserStepBack(par);
    return;
}

void P_ParserGetPrimary(PiParser *par) {
    P_ParserGetJustPrimary(par);
    int firsttime = 1;

    PiToken *tok = P_ParserReadToken(par);
    switch (tok->type) {
    case TOK_LBRACE:
        {
            int cnt = 0;
            while (1) {
                if (firsttime) {
                    tok = P_ParserReadToken(par);

                    if (tok->type == TOK_RBRACE) break;
                    else P_ParserStepBack(par);

                    firsttime = 0;
                }
                P_ParserGetMathExpr(par, 0);
                cnt += 4;

                tok = P_ParserReadToken(par);
                if (tok->type == TOK_RBRACE) break;
                else if (tok->type == TOK_COMMA) continue;

                P_ParserNewError(par, "',' or ')' expected");
                return;
            }

            P_ParserWrite8(par, OP_EXTCALL);
            P_ParserWrite32(par, cnt);
            return;
        }
    }

    P_ParserStepBack(par);
}

typedef struct {
    int tok1, tok2, op1, op2;
} PiMathExprDef;

// TODO: make expr levels with dYnAmIc size
PiMathExprDef mathexpr_tokens[] = {
    { TOK_EQUAL, TOK_EQUAL, OP_STORE, OP_STORE },
    { TOK_CMP_G, TOK_CMP_GE, OP_CMP_G, OP_CMP_GE },
    { TOK_CMP_EQ, TOK_CMP_NE, OP_CMP_EQ, OP_CMP_NE },
    { TOK_CMP_L, TOK_CMP_LE, OP_CMP_L, OP_CMP_LE },
    { TOK_PLUS, TOK_MINUS, OP_ADD, OP_SUB },
    { TOK_ASTERISK, TOK_SLASH, OP_MULT, OP_DIV },
    { TOK_MOD, TOK_MOD, OP_MOD, OP_MOD }
};

#define MATHEXPR_TOKENS_TOTAL (sizeof(mathexpr_tokens) / sizeof(*mathexpr_tokens))

void P_ParserWrite8(PiParser *par, int8_t x) {
    *par->out = x;

    //printf("At %d: %s\n", (par->out - par->out_start), opcode_strs[x]);
    par->out++;
}

void P_ParserWrite32(PiParser *par, int32_t x) {
    *(int32_t*)par->out = x;

    //printf("At %d: %d\n", (par->out - par->out_start), x);
    par->out += 4;
}

void P_ParserGetMathExpr(PiParser *par, int level) {
    if (level == MATHEXPR_TOKENS_TOTAL)
        return P_ParserGetPrimary(par);

    P_ParserGetMathExpr(par, level + 1);

    PiToken *tok = P_ParserReadToken(par);
    while (tok->type == mathexpr_tokens[level].tok1 || tok->type == mathexpr_tokens[level].tok2) {
        int type = tok->type;

        P_ParserGetMathExpr(par, level + 1);
        P_ParserWrite8(par, (type == mathexpr_tokens[level].tok1) ?
          mathexpr_tokens[level].op1 : mathexpr_tokens[level].op2
        );

        tok = P_ParserReadToken(par);
    }

    P_ParserStepBack(par);
}

void P_ParserNewError(PiParser *par, char *msg) {
    // printf("Error at %d:%d - %s.\n", par->buf[0].line, par->buf[0].col, msg);
    printf("Error at %d:%d - %s. | TOK = %s\n", par->buf[0].line, par->buf[0].col, msg, piTokenStrings[par->buf[par->loc - 1].type] );
}

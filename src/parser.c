#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"

void P_ParserCreate(PiParser *par, PiLexer *lexer) {
    par->lexer  = lexer;
    par->buf[0] = L_TokenNew(0, 0, 0, NULL);
    par->buf[1] = L_TokenNew(0, 0, 0, NULL);
    par->loc    = 2;
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
        case TOK_LBRACKET:
        printf("{\n");

        tok = P_ParserReadToken(par);
        if (tok->type == TOK_DEF) {
            printf("def:");
            while (1) {
                tok = P_ParserReadToken(par);
                if (tok->type != TOK_IDENT) {
                    P_ParserNewError(par, "Ident expected");
                    return;
                }
                printf(" %s", tok->str);

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
            printf(";\n");
        }
        else P_ParserStepBack(par);
        
        while (1) {
            tok = P_ParserReadToken(par);
            if (tok->type == TOK_RBRACKET)
                break;

            P_ParserStepBack(par);
            P_ParserGetStatement(par);
        }
        printf("}\n");
        return;

        case TOK_WHILE:
        tok = P_ParserReadToken(par);
        if (tok->type != TOK_LBRACE) {
            P_ParserNewError(par, "'(' expected");
            return;
        }
        
        printf("while (");
        P_ParserGetMathExpr(par, 0);
        printf(") ");

        tok = P_ParserReadToken(par);
        if (tok->type != TOK_RBRACE) {
            P_ParserNewError(par, "')' expected");
            return;
        }

        P_ParserGetStatement(par);
        return;
    }

    P_ParserStepBack(par);
    P_ParserGetMathExpr(par, 0);
    tok = P_ParserReadToken(par);
    if (tok->type != TOK_SEMICOLON) {
        P_ParserNewError(par, "';' expected");
        return;
    }
    printf(";\n");

    return;
}

void P_ParserGetJustPrimary(PiParser *par) {
    PiToken *tok = P_ParserReadToken(par);

    switch (tok->type) {
    case TOK_NUMBER:
        printf("num \"%s\"", tok->str);
        return;

    case TOK_IDENT:
        printf("ident \"%s\"", tok->str);
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
    }

    P_ParserStepBack(par);
    P_ParserNewError(par, "Unexpected token");
    return;
}

void P_ParserGetPrimary(PiParser *par) {
    P_ParserGetJustPrimary(par);
    int firsttime = 1;

    PiToken *tok = P_ParserReadToken(par);
    switch (tok->type) {
        case TOK_LBRACE:
        printf(" FUNCTION CALL with args:");

        while (1) {
            if (firsttime) {
            tok = P_ParserReadToken(par);
            
                if (tok->type == TOK_RBRACE) break;
                else P_ParserStepBack(par);

                firsttime = 0;
            }

            printf(" (");
            P_ParserGetMathExpr(par, 0);
            printf(")");

            tok = P_ParserReadToken(par);
            if (tok->type == TOK_RBRACE) break;
            else if (tok->type == TOK_COMMA) continue;

            P_ParserNewError(par, "',' or ')' expected");
            return;
        }
        return;
    }

    P_ParserStepBack(par);
}

typedef struct {
    int tok1, tok2;
} PiMathExprDef;

// TODO: make expr levels with dYnAmIc size
PiMathExprDef mathexpr_tokens[] = {
    { TOK_EQUAL, TOK_EQUAL },
    { TOK_PLUS, TOK_MINUS },
    { TOK_ASTERISK, TOK_SLASH },
    { TOK_MOD, TOK_MOD }
};

#define MATHEXPR_TOKENS_TOTAL (sizeof(mathexpr_tokens) / sizeof(*mathexpr_tokens))

void P_ParserGetMathExpr(PiParser *par, int level) {
    if (level == MATHEXPR_TOKENS_TOTAL)
        return P_ParserGetPrimary(par);

    P_ParserGetMathExpr(par, level + 1);

    PiToken *tok = P_ParserReadToken(par);
    while (tok->type == mathexpr_tokens[level].tok1 || tok->type == mathexpr_tokens[level].tok2) {
        printf(" %s ", piTokenStrings[tok->type]);
        
        P_ParserGetMathExpr(par, level + 1);
        tok = P_ParserReadToken(par);
    }

    P_ParserStepBack(par);
}

void P_ParserNewError(PiParser *par, char *msg) {
    printf("Error at %d:%d - %s.", par->buf[0].line, par->buf[0].col, msg);
}
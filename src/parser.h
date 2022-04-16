#ifndef TOYC_PARSER_H
#define TOYC_PARSER_H

#include "lexer.h"
#include <stdint.h>

typedef struct {
    char *str;
    int val;
} TcParserDef;

typedef struct {
    TcToken buf[2];
    int loc;

    char ok;
    TcLexer *lexer;
    char *out, *out_start;

    int defs_n;
    TcParserDef *defs;
} TcParser;

void P_ParserRevisit(TcParser *par);

void P_ParserCreate(TcParser *par, TcLexer *lexer, char *out);
void P_ParserDelete(TcParser *par);

TcToken *P_ParserReadToken(TcParser *par);
void     P_ParserStepBack(TcParser *par);

void P_ParserGetStatement(TcParser *par);
void P_ParserGetMathExpr(TcParser *par, int level);
void P_ParserNewError(TcParser *par, char *msg);

void P_ParserWrite8(TcParser *par, int8_t x);
void P_ParserWrite32(TcParser *par, int32_t x);

#endif // TOYC_PARSER_H

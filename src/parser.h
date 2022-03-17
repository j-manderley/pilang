#ifndef PILANG_PARSER_H
#define PILANG_PARSER_H

#include "lexer.h"
#include <stdint.h>

typedef struct {
    PiToken buf[2];
    int loc;

    PiLexer *lexer;
    char *out, *out_start;
} PiParser;

void P_ParserRevisit(PiParser *par);

void P_ParserCreate(PiParser *par, PiLexer *lexer, char *out);
void P_ParserDelete(PiParser *par);

PiToken *P_ParserReadToken(PiParser *par);
void     P_ParserStepBack(PiParser *par);

void P_ParserGetStatement(PiParser *par);
void P_ParserGetMathExpr(PiParser *par, int level);
void P_ParserNewError(PiParser *par, char *msg);

void P_ParserWrite8(PiParser *par, int8_t x);
void P_ParserWrite32(PiParser *par, int32_t x);

#endif // PILANG_PARSER_H

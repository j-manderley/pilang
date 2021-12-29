#ifndef PILANG_PARSER_H
#define PILANG_PARSER_H

#include "lexer.h"
#include <stdint.h>

/* Simple parser struct */

typedef struct {
    PiToken buf[2];
    int loc;

    PiLexer *lexer;
} PiParser;


/* This is where C-crap really starts */

typedef void (*PiNodeHandler)(void *node, int task, intmax_t arg);

enum {
    PI_NODE_PRINT,
    PI_NODE_DELETE
};

void P_ParserCreate(PiParser *par, PiLexer *lexer);
void P_ParserDelete(PiParser *par);

PiToken *P_ParserReadToken(PiParser *par);
void     P_ParserStepBack(PiParser *par);

void P_ParserGetStatement(PiParser *par);
void P_ParserGetMathExpr(PiParser *par, int level);
void P_ParserNewError(PiParser *par, char *msg);

#endif // PILANG_PARSER_H
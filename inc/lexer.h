#ifndef PILANG_LEXER_H
#define PILANG_LEXER_H

#include <stdbool.h>
#include <stdio.h>

#define FOR_EACH_TOKEN(f) \
    f(TOK_IDENT) \
    f(TOK_STRLITERAL) \
    f(TOK_NUMBER) \
    f(TOK_LBRACKET) \
    f(TOK_RBRACKET) \
    f(TOK_DEF) \
    f(TOK_COMMA) \
    f(TOK_SEMICOLON) \
    f(TOK_DEREF) \
    f(TOK_EQUAL) \
    f(TOK_LBRACE) \
    f(TOK_RBRACE) \
    f(TOK_MOD) \
    f(TOK_PLUS) \
    f(TOK_MINUS) \
    f(TOK_ASTERISK) \
    f(TOK_SLASH) \
    f(TOK_WHILE)

enum {
    #define f(name) name,
    FOR_EACH_TOKEN(f)
    #undef f
    TOK_EOF
};

typedef struct {
    char *str;
    int tok_id;
} PiKeywordDef;

typedef struct {
    int type;
    int line, col;
    char *str; // most of the time it`s NULL
} PiToken;

typedef struct {
	FILE *fptr;
	bool error;

	// internal crap
	int line, col;
	int prev_len;

	char buf[2];
    bool back;
} PiLexer;

extern PiKeywordDef piKeywordDefs[];
extern char *piTokenStrings[];

// Just sets str to NULL so no fancy memory leaks
PiToken L_TokenNew(int type, int line, int col, char *str);

// Just free(str)
void L_TokenDelete(PiToken *tok);

void L_LexerCreate(PiLexer *lexer, FILE *fptr);
void L_LexerDelete(PiLexer *lexer);

PiToken L_LexerReadToken(PiLexer *lexer);

#endif // PILANG_LEXER_H
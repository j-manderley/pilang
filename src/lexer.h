#ifndef TOYC_LEXER_H
#define TOYC_LEXER_H

#include <stdbool.h>
#include <stdio.h>

#define FOR_EACH_TOKEN(f) \
    f(TOK_IDENT) \
    f(TOK_STRLITERAL) \
    f(TOK_GLOB) \
    f(TOK_COMMA) \
    f(TOK_SEMICOLON) \
    f(TOK_NUMBER) \
    f(TOK_LBRACKET) \
    f(TOK_RBRACKET) \
    f(TOK_DEREF) \
    f(TOK_EQUAL) \
    f(TOK_RETURN) \
    f(TOK_LBRACE) \
    f(TOK_RBRACE) \
    f(TOK_MOD) \
    f(TOK_PLUS) \
    f(TOK_MINUS) \
    f(TOK_ASTERISK) \
    f(TOK_SLASH) \
    f(TOK_WHILE) \
    f(TOK_IF) \
    f(TOK_ELSE) \
    f(TOK_CMP_G) \
    f(TOK_CMP_EQ) \
    f(TOK_CMP_NE) \
    f(TOK_CMP_GE) \
    f(TOK_CMP_LE) \
    f(TOK_CMP_L) \
    f(TOK_NOT) \
    f(TOK_HASH)

enum {
    #define f(name) name,
    FOR_EACH_TOKEN(f)
    #undef f
    TOK_EOF
};

typedef struct {
    char *str;
    int tok_id;
} TcKeywordDef;

typedef struct {
    int type;
    int line, col;
    char *str; // most of the time it`s NULL
} TcToken;

typedef struct {
	FILE *fptr;
	bool error;

	// internal crap
	int line, col;
	int prev_len;

	char buf[2];
    bool back;
} TcLexer;

extern TcKeywordDef tcKeywordDefs[];
extern char *tcTokenStrings[];

// Just sets str to NULL so no fancy memory leaks
TcToken L_TokenNew(int type, int line, int col, char *str);

// Just free(str)
void L_TokenDelete(TcToken *tok);

void L_LexerCreate(TcLexer *lexer, FILE *fptr);
void L_LexerDelete(TcLexer *lexer);

TcToken L_LexerReadToken(TcLexer *lexer);

#endif // TOYC_LEXER_H

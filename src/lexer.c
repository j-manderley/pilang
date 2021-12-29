#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

PiKeywordDef PiKeywordDefs[] = {
	// zzzzzzz
};

char *piTokenStrings[] = {
	#define f(name) #name,
	FOR_EACH_TOKEN(f)
	#undef f
	"TOK_EOF"
};

void L_LexerCreate(PiLexer *lexer, FILE *fptr) {
	memset(lexer, 0, sizeof(*lexer));
	lexer->fptr = fptr;
}

// TODO: delete this?????
void L_LexerDelete(PiLexer *lexer) {
	// for now it is useless
}

PiToken L_TokenNew(int type, int line, int col, char *str) {
	PiToken tok;

	tok.type = type;
	tok.line = line;
	tok.col = col;
	tok.str = str;

	return tok;
}

void L_TokenDelete(PiToken *tok) {
	free(tok->str);
}

PiToken L_TokenError(PiLexer *lexer, char *msg) {
	return L_TokenNew(TOK_EOF, lexer->line, lexer->col, msg);
}

PiToken L_LexerCreateToken(PiLexer *lexer, int type, char *str) {
	return L_TokenNew(type, lexer->line, lexer->col - 1, str);
}

char L_LexerReadChar(PiLexer *lexer) {
	char c = fgetc(lexer->fptr);

	if (c == '\n') {
		lexer->line++;
		lexer->prev_len = lexer->col;
		lexer->col = 0;
	}
	else lexer->col++;

	return c;
}

void L_LexerStepBack(PiLexer *lexer) {
	fseek(lexer->fptr, -1, SEEK_CUR);
	char c = fgetc(lexer->fptr);
	fseek(lexer->fptr, -1, SEEK_CUR);

	if (c == '\n') {
		lexer->line--;
		lexer->col = lexer->prev_len;
		lexer->prev_len = -1; // why?
	}
	else lexer->col--;
}

PiToken L_LexerReadNumber(PiLexer *lexer) {
	PiBuffer8 buf = U_BufferNew8(0);

	char c;
	while (c = L_LexerReadChar(lexer), isdigit(c))
		U_BufferAppend8(&buf, c);

	if (c == '.') {
		U_BufferAppend8(&buf, '.');
		while (c = L_LexerReadChar(lexer), isdigit(c))
			U_BufferAppend8(&buf, c);
	}

	U_BufferAppend8(&buf, '\0');
	U_BufferFit8(&buf);
	L_LexerStepBack(lexer);

	return L_TokenNew(TOK_NUMBER, lexer->line, lexer->col, buf.ptr);
}

PiKeywordDef piKeywordDefs[] = {
	{ "while", TOK_WHILE },
	{ "def", TOK_DEF }
};

#define PI_KEYWORD_DEFS_TOTAL (sizeof(piKeywordDefs) / sizeof(*piKeywordDefs))

PiToken L_LexerReadIdent(PiLexer *lexer) {
	PiBuffer8 buf = U_BufferNew8(1);
	buf.ptr[0] = L_LexerReadChar(lexer);

	char c;
	while (c = L_LexerReadChar(lexer), isalnum(c) || c == '_')
		U_BufferAppend8(&buf, c);

	U_BufferAppend8(&buf, '\0');
	U_BufferFit8(&buf);
	L_LexerStepBack(lexer);

	for (int i = 0; i < PI_KEYWORD_DEFS_TOTAL; i++) {
		if (!strcmp(buf.ptr, piKeywordDefs[i].str)) {
			U_BufferFree(&buf);
			return L_TokenNew(piKeywordDefs[i].tok_id, lexer->line, lexer->col, NULL);
		}
	}

	return L_TokenNew(TOK_IDENT, lexer->line, lexer->col, buf.ptr);
}

char hex2char(int h) {
	return (h < 10) ? ('0' + h) : ('A' + h - 10);
}

PiToken L_LexerReadToken(PiLexer *lexer) {
try_again:;
	char c = L_LexerReadChar(lexer);

	switch (c) {
		// Separators
		case ',':
		return L_LexerCreateToken(lexer, TOK_COMMA, NULL);
		case '}':
		return L_LexerCreateToken(lexer, TOK_RBRACKET, NULL);
		case '{':
		return L_LexerCreateToken(lexer, TOK_LBRACKET, NULL);
		case '%':
		return L_LexerCreateToken(lexer, TOK_MOD, NULL);
		case '(':
		return L_LexerCreateToken(lexer, TOK_LBRACE, NULL);
		case ')':
		return L_LexerCreateToken(lexer, TOK_RBRACE, NULL);
		case ';':
		return L_LexerCreateToken(lexer, TOK_SEMICOLON, NULL);
		case '$':
		return L_LexerCreateToken(lexer, TOK_DEREF, NULL);
		case '=':
		return L_LexerCreateToken(lexer, TOK_EQUAL, NULL);
		case '+':
		return L_LexerCreateToken(lexer, TOK_PLUS, NULL);
		case '-':
		return L_LexerCreateToken(lexer, TOK_MINUS, NULL);
		case '*':
		return L_LexerCreateToken(lexer, TOK_ASTERISK, NULL);
		case '/':
		return L_LexerCreateToken(lexer, TOK_SLASH, NULL);

		case ' ':
		case '\t':
		case '\n':
			goto try_again;

		case EOF:
		return L_LexerCreateToken(lexer, TOK_EOF, NULL);
	}

	if (c == '_' || isalpha(c)) {
		L_LexerStepBack(lexer);
		return L_LexerReadIdent(lexer);
	}
	else if (c == '.' || isdigit(c)) {
		L_LexerStepBack(lexer);
		return L_LexerReadNumber(lexer);
	}

	char *err_msg = strdup("Invalid character (0xXX) \'x\'");
	err_msg[21] = hex2char((c >> 4) % 16);
	err_msg[22] = hex2char(c % 16);
	err_msg[26] = (c >= 32) ? c : ' ';

	return L_TokenError(lexer, err_msg);
}

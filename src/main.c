#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Specify filename\n");
		return 0;
	}

	FILE *fptr = fopen(argv[1], "r");
	if (!fptr) {
		printf("Trying to open file: %s\n", strerror(errno));
		return -1;
	}

	PiLexer lexer;
	L_LexerCreate(&lexer, fptr);

	PiParser par;
	P_ParserCreate(&par, &lexer);

	P_ParserGetStatement(&par);

	P_ParserDelete(&par);
	L_LexerDelete(&lexer);
	fclose(lexer.fptr);
}
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "lexer.h"
#include "machine.h"
#include "parser.h"

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

    char *prog = malloc(512);

    PiLexer lex;
    PiParser par;
    PiMachine vm;

    L_LexerCreate(&lex, fptr);
    P_ParserCreate(&par, &lex, prog);
    P_ParserGetStatement(&par);
    P_ParserRevisit(&par);

    VM_Create(&vm, 512, 512, prog);
    VM_Execute(&vm, 0);

    VM_Destroy(&vm);
    P_ParserDelete(&par);
    L_LexerDelete(&lex);

    fclose(fptr);
}

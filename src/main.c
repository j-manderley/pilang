#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "lexer.h"
#include "machine.h"
#include "parser.h"

#ifdef _WIN32
    #include <windows.h>
#endif

int main(int argc, char **argv) {
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif

    if (argc != 2) {
        printf("Specify filename\n");
        return 0;
    }

    FILE *fptr = fopen(argv[1], "r");
    if (!fptr) {
        printf("Trying to open file: %s\n", strerror(errno));
        return -1;
    }

    char *prog = malloc(1024*1024);

    TcLexer lex;
    TcParser par;
    TcMachine vm;

    L_LexerCreate(&lex, fptr);
    P_ParserCreate(&par, &lex, prog);
    
    while (1) {
        TcToken *tok = P_ParserReadToken(&par);
        if (tok->type == TOK_EOF)
            break;

        P_ParserStepBack(&par);
        P_ParserGetStatement(&par);
    }

    if (*par.out != OP_RETURN)
        P_ParserWrite8(&par, OP_RETURN);

    P_ParserRevisit(&par);

    if (par.ok) {
        VM_Create(&vm, 1024*1024, 1024*1024, prog);
        VM_Execute(&vm, 0);
        VM_Destroy(&vm);
    }

    P_ParserDelete(&par);
    L_LexerDelete(&lex);

    fclose(fptr);
}

{
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-Iinc"
        }
    },
    files = {
        "src/main.c"
    },
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/release/src/main.c.o: src/main.c  inc/lexer.h inc/parser.h inc/lexer.h\
"
}
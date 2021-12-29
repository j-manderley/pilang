{
    files = {
        "src/lexer.c"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-Iinc"
        }
    },
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/release/src/lexer.c.o: src/lexer.c  inc/lexer.h inc/utils.h\
"
}
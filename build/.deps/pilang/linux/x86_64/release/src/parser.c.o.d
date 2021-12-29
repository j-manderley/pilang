{
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-Iinc"
        }
    },
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/release/src/parser.c.o: src/parser.c  inc/parser.h inc/lexer.h inc/lexer.h\
",
    files = {
        "src/parser.c"
    }
}
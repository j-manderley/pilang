{
    files = {
        "src/lexer.c"
    },
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/debug/src/lexer.c.o: src/lexer.c  src/lexer.h src/utils.h\
",
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-g",
            "-O0",
            "-Isrc",
            "-DDEBUG"
        }
    }
}
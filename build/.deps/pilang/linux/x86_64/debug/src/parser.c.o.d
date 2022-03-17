{
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/debug/src/parser.c.o: src/parser.c  src/parser.h src/lexer.h src/machine.h\
",
    files = {
        "src/parser.c"
    },
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
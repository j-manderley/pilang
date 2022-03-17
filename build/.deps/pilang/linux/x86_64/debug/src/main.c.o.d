{
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/debug/src/main.c.o: src/main.c  src/lexer.h src/machine.h src/parser.h\
",
    files = {
        "src/main.c"
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
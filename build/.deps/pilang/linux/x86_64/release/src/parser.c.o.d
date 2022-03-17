{
    files = {
        "src/parser.c"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-fvisibility=hidden",
            "-O3",
            "-Isrc",
            "-DNDEBUG"
        }
    },
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/release/src/parser.c.o: src/parser.c  src/parser.h src/lexer.h src/machine.h\
"
}
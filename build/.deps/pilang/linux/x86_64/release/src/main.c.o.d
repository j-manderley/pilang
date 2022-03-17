{
    files = {
        "src/main.c"
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
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/release/src/main.c.o: src/main.c  src/lexer.h src/machine.h src/parser.h\
"
}
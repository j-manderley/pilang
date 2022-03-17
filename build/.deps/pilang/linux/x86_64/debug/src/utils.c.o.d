{
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/debug/src/utils.c.o: src/utils.c  src/utils.h\
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
    },
    files = {
        "src/utils.c"
    }
}
{
    files = {
        "src/machine.c"
    },
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/debug/src/machine.c.o: src/machine.c  src/machine.h\
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
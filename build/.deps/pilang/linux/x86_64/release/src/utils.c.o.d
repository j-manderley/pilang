{
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-Iinc"
        }
    },
    files = {
        "src/utils.c"
    },
    depfiles_gcc = "build/.objs/pilang/linux/x86_64/release/src/utils.c.o: src/utils.c  inc/utils.h\
"
}
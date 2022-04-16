#include "utils.h"
#include <stdlib.h>
#include <string.h> // memset

TC_IMPL_BUFFER(8, char)
TC_IMPL_BUFFER(32, int)

void U_BufferFree(void *bufptr) {
    free(((TcBuffer*)bufptr)->ptr);
    memset(bufptr, 0, sizeof(TcBuffer));
}

#include "utils.h"
#include <stdlib.h>
#include <string.h> // memset

PI_IMPL_BUFFER(8, char)
PI_IMPL_BUFFER(32, int)

void U_BufferFree(void *bufptr) {
    free(((PiBuffer*)bufptr)->ptr);
    memset(bufptr, 0, sizeof(PiBuffer));
}

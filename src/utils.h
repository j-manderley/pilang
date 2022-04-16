#ifndef TOYC_UTILS_H
#define TOYC_UTILS_H

#include <stdint.h>

typedef struct {
	void *ptr;
	int size;
	int capacity;
} TcBuffer;

#define TC_DECL_BUFFER(suffix, type) \
	typedef struct { \
		type *ptr; \
		int size; \
		int capacity; \
	} TcBuffer##suffix; \
	 \
	TcBuffer##suffix U_BufferNew##suffix(int size); \
	void U_BufferFit##suffix(TcBuffer##suffix *buf); \
	void U_BufferAppend##suffix(TcBuffer##suffix *buf, int c);

#define TC_IMPL_BUFFER(suffix, type) \
	TcBuffer##suffix U_BufferNew##suffix(int size) { \
		TcBuffer##suffix buf; \
		\
		buf.ptr = malloc(size * sizeof(*buf.ptr)); \
		buf.size = size; \
		buf.capacity = size; \
		\
		return buf; \
	} \
	\
	void U_BufferFit##suffix(TcBuffer##suffix *buf) { \
		buf->capacity = buf->size; \
		buf->ptr = realloc(buf->ptr, buf->capacity * sizeof(*buf->ptr)); \
	} \
	\
	void U_BufferAppend##suffix(TcBuffer##suffix *buf, int c) { \
		if (buf->capacity == 0) buf->capacity = 1; \
		\
		while (buf->size >= buf->capacity) \
			buf->capacity = (buf->capacity * 3 + 1) / 2; \
		\
		buf->ptr = realloc(buf->ptr, buf->capacity * sizeof(*buf->ptr)); \
		buf->ptr[buf->size] = c; \
		buf->size++; \
	}

TC_DECL_BUFFER(8, char)
TC_DECL_BUFFER(32, int)

void U_BufferFree(void *bufptr);

#endif // TOYC_UTILS_H

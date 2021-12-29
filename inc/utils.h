#ifndef PILANG_UTILS_H
#define PILANG_UTILS_H

typedef struct {
	void *ptr;
	int size;
	int capacity;
} PiBuffer;

#define PI_DECL_BUFFER(suffix, type) \
	typedef struct { \
		type *ptr; \
		int size; \
		int capacity; \
	} PiBuffer##suffix; \
	 \
	PiBuffer##suffix U_BufferNew##suffix(int size); \
	void U_BufferFit##suffix(PiBuffer##suffix *buf); \
	void U_BufferAppend##suffix(PiBuffer##suffix *buf, char c);

#define PI_IMPL_BUFFER(suffix, type) \
	PiBuffer##suffix U_BufferNew##suffix(int size) { \
		PiBuffer##suffix buf; \
		\
		buf.ptr = malloc(size * sizeof(*buf.ptr)); \
		buf.size = size; \
		buf.capacity = size; \
		\
		return buf; \
	} \
	\
	void U_BufferFit##suffix(PiBuffer##suffix *buf) { \
		buf->capacity = buf->size; \
		buf->ptr = realloc(buf->ptr, buf->capacity * sizeof(*buf->ptr)); \
	} \
	\
	void U_BufferAppend##suffix(PiBuffer##suffix *buf, char c) { \
		if (buf->capacity == 0) buf->capacity = 1; \
		\
		while (buf->size >= buf->capacity) \
			buf->capacity = (buf->capacity * 3 + 1) / 2; \
		\
		buf->ptr = realloc(buf->ptr, buf->capacity * sizeof(*buf->ptr)); \
		buf->ptr[buf->size] = c; \
		buf->size++; \
	}

PI_DECL_BUFFER(8, char)
PI_DECL_BUFFER(32, int)

void U_BufferFree(void *bufptr);

#endif // PILANG_UTILS_H
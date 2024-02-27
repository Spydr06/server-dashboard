#include "dashboard.h"
#include <stdint.h>

size_t strlen(const char* str) {
    size_t i = 0;
    while(str[i]) i++;
    return i;
}

char* strcpy(char* restrict dest, const char* src) {
    char* dest_bytes = dest;
    while(*src)
        *(dest_bytes++) = *(src++);
    *dest_bytes = 0;
    return dest;
}


char* strncpy(char* restrict dest, const char* src, size_t max_size) {
    char* dest_bytes = dest;

    size_t i = 0;
    while(*src && i < max_size) {
        *(dest_bytes++) = *(src++);
        i++;
    }
    while(i < max_size) {
        *(dest_bytes++) = 0;
        i++;
    }

    return dest;
}

char* strcat(char* restrict dest, const char* restrict src) {
    strcpy(dest + strlen(dest), src);
    return dest;
}

char* strncat(char* restrict dest, const char* restrict src, size_t max_size) {
    char* dest_bytes = dest;
    dest_bytes += strlen(dest);
    size_t i = 0;
    while(*src && i < max_size) {
        *(dest_bytes++) = *(src++);
        i++;
    }
    *dest_bytes = 0;
    return dest;
}

char* stpncpy(char* restrict dest, const char* src, size_t n) {
    size_t nulls, copied, src_len = strlen(src);
	if (n >= src_len) {
		nulls = n - src_len;
		copied = src_len;
	} else {
		nulls = 0;
		copied = n;
	}

	memcpy(dest, src, copied);
	memset(dest + src_len, 0, nulls);
	return dest + n - nulls;
}

void* memset(void* restrict s, int c, size_t n) {
    uint8_t* p = s;
    while(n--)
        *p++ = (uint8_t) c;
    return s;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    for(size_t i = 0; i < n; i++)
        ((uint8_t*) dest)[i] = ((uint8_t*) src)[i];
    return dest;
}


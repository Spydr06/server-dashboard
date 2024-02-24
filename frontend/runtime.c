#include "dashboard.h"

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


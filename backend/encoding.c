#include "encoding.h"

#include <stdio.h>
#include <string.h>

void url_decode(char* decoded, const char* src) {
    size_t src_len = strlen(src), decoded_len = 0;

    for(size_t i = 0; i < src_len; i++) {
        if(src[i] == '%' && i + 2 < src_len) {
            int hex_val;
            sscanf(src + i + 1, "%2x", &hex_val);
            decoded[decoded_len++] = hex_val;
            i += 2;
        }
        else
            decoded[decoded_len++] = src[i];
    }

    decoded[decoded_len] = '\0';
}

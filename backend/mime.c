#include "mime.h"

#include <ctype.h>
#include <string.h>

#define DEFAULT_MIME_TYPE "application/octet-stream"

static char* strlower(char* str) {
    for(char* p = str; *p; p++)
        *p = tolower(*p);
    return str;
}

char* get_mime_type(char* filename) {
    char* ext = strrchr(filename, '.');
    if(!ext)
        return DEFAULT_MIME_TYPE;

    ext++;
    strlower(ext);

    if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) return "text/html";
    if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0) return "image/jpg";
    if (strcmp(ext, "css") == 0) return "text/css";
    if (strcmp(ext, "js") == 0) return "application/javascript";
    if (strcmp(ext, "json") == 0) return "application/json";
    if (strcmp(ext, "txt") == 0) return "text/plain";
    if (strcmp(ext, "gif") == 0) return "image/gif";
    if (strcmp(ext, "png") == 0) return "image/png";

    return DEFAULT_MIME_TYPE;
}

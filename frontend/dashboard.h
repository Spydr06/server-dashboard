#ifndef _DASHBOARD_H
#define _DASHBOARD_H

#include <stdint.h>
#include <stddef.h>

#include "../api/api.h"

#define _(str) str

#define LEN(arr) (sizeof((arr)) / sizeof((arr)[0]))

#define JS_STR(str) (str), (strlen((str)))
#define const_JS_STR(str) (str), (LEN((str)) - 1)

#define UNREACHABLE() __builtin_unreachable()

#define CONCAT(buf, str, buf_len) do {              \
        char* s = stpncpy((buf), (str), (buf_len)); \
        (buf_len) -= s - (buf);                     \
        (buf) = s;                                  \
    } while(0)

#define CONCAT_ALL(buf, buf_len, ...) do {          \
        const char* const strs[] = { __VA_ARGS__ }; \
        for(size_t i = 0; i < LEN(strs); i++)       \
            CONCAT(buf, strs[i], buf_len);          \
    } while(0)

#define HTML_BUFSIZ 8192
#define LABEL_BUFSIZ 32

// error values
#define E_SUCCESS 0
#define E_404     1
#define E_NULL    2

typedef enum widget_class : uint8_t {
    WCLASS_GENERIC,
    WCLASS_QUICKSETTING,
    WCLASS_SERVICE,
    __WCLASS_NUM,
} widget_class_t;

typedef enum content_type : uint8_t {
    CONTENT_GRAPH,
    CONTENT_LABEL,
    CONTENT_SERVICE_STATUS
} content_type_t;

typedef struct service {
    const char* name;
    uint64_t mem_usage;
    uint32_t cpu_time_ms;
    enum : uint8_t {
        SERVICE_STATUS_UNKNOWN,
        SERVICE_STATUS_UP,
        SERVICE_STATUS_DOWN,
        __SERVICE_STATUS_NUM
    } status;
} service_t;

typedef struct graph {
    uint8_t resolution;
} graph_t;

typedef struct label {
    char buffer[LABEL_BUFSIZ];
} label_t;

typedef struct widget widget_t;

typedef void (*html_generator_function_t)(const widget_t* widget, char** buf, size_t* buf_len);

struct widget {
    const char* title;
    const char* html_id;

    widget_class_t class;
 
    html_generator_function_t html_generator;
    content_type_t content_type;
    union {
        graph_t graph;
        label_t label;
        service_t service;
    } content;
};

// JS callbacks
extern int set_inner_html(const char* restrict element_id, size_t element_id_len, const char* restrict html, size_t html_len);
extern int api_request(const char* restrict endpoint, size_t endpoint_len, uint8_t* restrict response_buffer, size_t response_buffer_len);

// functions from `string.h`
size_t strlen(const char* str);

char* strcpy(char* restrict dest, const char* src);
char* strncpy(char* restrict dest, const char* src, size_t max_size);
char* stpncpy(char* restrict dest, const char* src, size_t max_size);

char* strcat(char* restrict dest, const char* restrict src);
char* strncat(char* restrict dest, const char* restrict src, size_t max_size);

void* memcpy(void* restrict dest, const void* restrict src, size_t n);
void* memset(void* restrict s, int c, size_t n);

#endif /* _DASHBOARD_H */


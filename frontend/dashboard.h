#ifndef _DASHBOARD_H
#define _DASHBOARD_H

#include <stdint.h>
#include <stddef.h>

#define _(str) str

#define LEN(arr) (sizeof((arr)) / sizeof((arr)[0]))
#define JS_STR(str) (str), (strlen((str)))

#define UNREACHABLE() __builtin_unreachable()

typedef enum widget_class : uint8_t {
    WCLASS_GENERIC,
    WCLASS_QUICKSETTING,
    WCLASS_SERVICE,
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
        SERVICE_STATUS_DOWN
    } status;
} service_t;

typedef struct widget {
    const char* title;

    widget_class_t class;
    content_type_t content_type;
    union {
        struct {

        } content_graph;
        struct {
            const char* label; 
        } content_label;
        service_t content_service;
    };
} widget_t;

extern void create_widget(int32_t id, const char* restrict title, size_t title_len, widget_class_t class);
extern void update_widget_content(int32_t id, const char* restrict html, size_t html_len);

// functions from `string.h`
size_t strlen(const char* str);

char* strcpy(char* restrict dest, const char* src);
char* strncpy(char* restrict dest, const char* src, size_t max_size);

char* strcat(char* restrict dest, const char* restrict src);
char* strncat(char* restrict dest, const char* restrict src, size_t max_size);

#endif /* _DASHBOARD_H */


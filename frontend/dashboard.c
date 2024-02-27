#include "dashboard.h"

#define SERVICE_WIDGET(name, html_id, _service) {                   \
    (name), "widget-service-" html_id,                              \
    WCLASS_SERVICE, generate_service_widget,                        \
    CONTENT_SERVICE_STATUS,                                         \
    { .service = { (_service), 0.0, 0, SERVICE_STATUS_UNKNOWN } }   \
}

#define LABEL_WIDGET(name, html_id, wclass, _label) {   \
    (name), "widget-" html_id,                          \
    WCLASS_##wclass, generate_label_widget,             \
    CONTENT_LABEL,                                      \
    { .label = { _label } }                             \
}

static void generate_service_widget(const widget_t* widget, char** buf, size_t* buf_len);
static void generate_label_widget(const widget_t* widget, char** buf, size_t* buf_len);
static void generate_graph_widget(const widget_t* widget, char** buf, size_t* buf_len);


static widget_t widgets[] = {
    {
        "Greeting", "widget-greeting",
        WCLASS_QUICKSETTING,
        NULL,
        CONTENT_LABEL,
        {}
    },
    {
        "Weather", "widget-weather",
        WCLASS_QUICKSETTING,
        NULL,
        CONTENT_LABEL,
        {}
    },
    {
        "Power Options", "widget-power-opt",
        WCLASS_QUICKSETTING,
        NULL,
        CONTENT_LABEL,
        {}
    },
    LABEL_WIDGET("Uptime", "uptime", QUICKSETTING, "00d 00:00h"),
    {
        "Ping", "widget-ping",
        WCLASS_GENERIC,
        NULL,
        CONTENT_GRAPH,
        {}
    },
    {
        "Processor Usage", "widget-cpu-usage",
        WCLASS_GENERIC,
        NULL,
        CONTENT_GRAPH,
        {}
    },
    {
        "Memory Usage", "widget-mem-usage",
        WCLASS_GENERIC,
        NULL,
        CONTENT_GRAPH,
        {}
    },
    {
        "Disk Usage", "widget-disk-usage",
        WCLASS_GENERIC,
        NULL,
        CONTENT_GRAPH,
        {}
    },
    {
        "Swap Usage", "widget-swap-usage",
        WCLASS_GENERIC,
        NULL,
        CONTENT_GRAPH,
        {}
    },
    {
        "TCP Connections", "widget-tcp",
        WCLASS_GENERIC,
        NULL,
        CONTENT_GRAPH,
        {}
    },
    {
        "UDP Connections", "widget-udp",
        WCLASS_GENERIC,
        NULL,
        CONTENT_GRAPH,
        {}
    },
    SERVICE_WIDGET("Gitea", "gitea", "gitea"),
    SERVICE_WIDGET("Creative Server", "mc-c", "minecraft@main"),
    SERVICE_WIDGET("Survival Server", "mc-s", "minecraft@survival")
};

static const char* const wclass_containers[__WCLASS_NUM] = {
    [WCLASS_GENERIC] = "generic-widget-container",
    [WCLASS_SERVICE] = "service-widget-container",
    [WCLASS_QUICKSETTING] = "quicksetting-widget-container"
};

static const char* const wclass_css_class[__WCLASS_NUM] = {
    [WCLASS_GENERIC] = "widget-generic",
    [WCLASS_SERVICE] = "widget-service",
    [WCLASS_QUICKSETTING] = "widget-quicksetting"
};

static const char* const service_status_css_class[__SERVICE_STATUS_NUM] = {
    [SERVICE_STATUS_DOWN] = "status-down",
    [SERVICE_STATUS_UP] = "status-up",
    [SERVICE_STATUS_UNKNOWN] = "status-unknown"
};

static void create_widget(const widget_t* widget, char* buf, size_t buf_len)
{
    CONCAT_ALL(buf, buf_len,
        "<div class='widget ", wclass_css_class[widget->class], "' id='", widget->html_id, "'>"
        "<b class='widget-title'>", widget->title, "</b>"
        "<div id='", widget->html_id, "-content' class='widget-content ", wclass_css_class[widget->class], "-content'>"
    );

    if(widget->html_generator)
        widget->html_generator(widget, &buf, &buf_len);

    CONCAT(buf, "</div></div>", buf_len);
}

static void generate_service_widget(const widget_t* widget, char** buf, size_t* buf_len)
{
    CONCAT_ALL(*buf, *buf_len,
        "<div class='service-status-labels'>"
            "<label>cpu time:</label>"
            "<b>0 ms</b>"
            "<label>memory:</label>"
            "<b>0 B</b>"
        "</div>"
        "<div class='", service_status_css_class[widget->content.service.status], " status-icon'></div>"
    );
}

static void generate_label_widget(const widget_t* widget, char** buf, size_t* buf_len)
{
    CONCAT(*buf, "<div class='widget-label-container'><label class='widget-label'>", *buf_len);
    CONCAT(*buf, widget->content.label.buffer, *buf_len);
    CONCAT(*buf, "</label></div>", *buf_len);
}

extern void start(void) {
    static char html_buf[HTML_BUFSIZ];
    
    for(widget_class_t wclass = WCLASS_GENERIC; wclass < __WCLASS_NUM; wclass++)
    {
        char* ptr = html_buf;
        for(size_t i = 0, capacity = LEN(html_buf) - 1; i < LEN(widgets); i++)
        {
            const widget_t* widget = &widgets[i];
            if(widget->class != wclass)
                continue;
            create_widget(widget, ptr, capacity);

            size_t len = strlen(ptr);
            capacity -= len;
            ptr += len;
        }

        set_inner_html(JS_STR(wclass_containers[wclass]), html_buf, (size_t) (ptr - html_buf));
    }
}

extern int update(void) {
    static uint8_t response_buffer[sizeof(api_request_status_t) + 1024];

    int res;
    if((res = api_request(JS_STR(STATUS_REQUEST_ENDPOINT), response_buffer, LEN(response_buffer))) != 0)
        return res;

    return E_SUCCESS;
}

static void status_request_callback(uint8_t* restrict response, size_t response_len)
{

}

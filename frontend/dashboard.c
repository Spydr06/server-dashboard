#include "dashboard.h"

#define SERVICE_WIDGET(name, html_id, service) \
    { (name), "widget-service-" html_id, WCLASS_SERVICE, CONTENT_SERVICE_STATUS, { .content_service = { (service), 0.0, 0, SERVICE_STATUS_UNKNOWN } } }

static widget_t widgets[] = {
    {
        "Greeting", "widget-greeting",
        WCLASS_QUICKSETTING,
        CONTENT_LABEL,
        {}
    },
    {
        "Weather", "widget-weather",
        WCLASS_QUICKSETTING,
        CONTENT_LABEL,
        {}
    },
    {
        "Power Options", "widget-power-opt",
        WCLASS_QUICKSETTING,
        CONTENT_LABEL,
        {}
    },
    { 
        "Uptime", "widget-uptime",
        WCLASS_QUICKSETTING,
        CONTENT_LABEL,
        {}
    },
    {
        "Ping", "widget-ping",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "Processor Usage", "widget-cpu-usage",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "Memory Usage", "widget-mem-usage",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "Disk Usage", "widget-disk-usage",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "Swap Usage", "widget-swap-usage",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "TCP Connections", "widget-tcp",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "UDP Connections", "widget-udp",
        WCLASS_GENERIC,
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

#define CONCAT(buf, str, buf_len) do {              \
        char* s = stpncpy((buf), (str), (buf_len)); \
        (buf_len) -= s - (buf);                     \
        (buf) = s;                                  \
    } while(0)

static void create_widget(const widget_t* widget, char* buf, size_t buf_len)
{
    CONCAT(buf, "<div class='widget ", buf_len);
    CONCAT(buf, wclass_css_class[widget->class], buf_len);
    CONCAT(buf, "' id='", buf_len);
    CONCAT(buf, widget->html_id, buf_len);
    CONCAT(buf, "'><b class='widget-title'>", buf_len);
    CONCAT(buf, widget->title, buf_len);
    CONCAT(buf, "</b><div id='", buf_len);
    CONCAT(buf, widget->html_id, buf_len);
    CONCAT(buf, "-content' class='widget-content ", buf_len);
    CONCAT(buf, wclass_css_class[widget->class], buf_len);
    CONCAT(buf, "-content'>", buf_len);

    switch(widget->content_type)
    {
    case CONTENT_GRAPH:
        break;
    case CONTENT_LABEL:
        break;
    case CONTENT_SERVICE_STATUS:
        CONCAT(buf, "<div class='service-status-labels'><label>cpu time:</label><b>0 ms</b><label>memory:</label><b>0 B</b></div><div class='", buf_len);
        switch(widget->content_service.status)
        {
        case SERVICE_STATUS_UP:
            CONCAT(buf, "status-up", buf_len);
            break;
        case SERVICE_STATUS_DOWN:
            CONCAT(buf, "status-down", buf_len);
            break;
        case SERVICE_STATUS_UNKNOWN:
            CONCAT(buf, "status-unknown", buf_len);
            break;
        default:
            UNREACHABLE();
        }
        CONCAT(buf, " status-icon'></div>", buf_len);
        break;
    default:
        UNREACHABLE();
    }

    CONCAT(buf, "</div></div>", buf_len);
}

extern void start(void) {
    static char html_buf[1024 * LEN(widgets)];
    
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

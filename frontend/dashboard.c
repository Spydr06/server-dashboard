#include "dashboard.h"

#define SERVICE_WIDGET(name, service) \
    { (name), WCLASS_SERVICE, CONTENT_SERVICE_STATUS, { .content_service = { (service), 0.0, 0, SERVICE_STATUS_UNKNOWN } } }

static widget_t widgets[] = {
    {
        "Power Options",
        WCLASS_QUICKSETTING,
        CONTENT_LABEL,
        {}
    },
    { 
        "Uptime",
        WCLASS_QUICKSETTING,
        CONTENT_LABEL,
        {}
    },
    {
        "Ping",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "Processor Usage",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "Memory Usage",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "Disk Usage",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "Swap Usage",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "TCP Connections",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    {
        "UDP Connections",
        WCLASS_GENERIC,
        CONTENT_GRAPH,
        {}
    },
    SERVICE_WIDGET("Gitea", "gitea"),
    SERVICE_WIDGET("Creative Server", "minecraft@main"),
    SERVICE_WIDGET("Survival Server", "minecraft@survival")
};

void widget_build_html(const widget_t* widget, char* buf, const size_t buf_len) {
    switch(widget->content_type)
    {
    case CONTENT_GRAPH:
        break;
    case CONTENT_LABEL:
        break;
    case CONTENT_SERVICE_STATUS:
        strncpy(buf, "<div class='service-status-labels'><label>cpu time:</label><b>0 ms</b><label>memory:</label><b>0 B</b></div><div class='", buf_len);
        switch(widget->content_service.status)
        {
        case SERVICE_STATUS_UP:
            strncat(buf, "status-up", buf_len);
            break;
        case SERVICE_STATUS_DOWN:
            strncat(buf, "status-down", buf_len);
            break;
        case SERVICE_STATUS_UNKNOWN:
            strncat(buf, "status-unknown", buf_len);
            break;
        default:
            UNREACHABLE();
        }
        strncat(buf, " status-icon'></div>", buf_len);
        break;
    default:
        UNREACHABLE();
    }
}

static void update_widget_data(widget_t* widget)
{
    switch(widget->class)
    {
    case WCLASS_QUICKSETTING:
        break;
    case WCLASS_GENERIC:
        break;
    case WCLASS_SERVICE:
        break;
    default:
        UNREACHABLE();
    }
}

extern void start(void) {
    static char html_buf[1024];

    for(size_t i = 0; i < LEN(widgets); i++)
    {
        widget_t* widget = &widgets[i];
        create_widget(i, JS_STR(widget->title), widget->class);
        
        update_widget_data(widget);

        widget_build_html(widget, html_buf, LEN(html_buf));
        update_widget_content(i, JS_STR(html_buf));
    }
}



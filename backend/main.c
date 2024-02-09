#include "server.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define DEFAULT_PORT 6969

#define _(str) str

_Noreturn void panic(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fflush(stderr);
    va_end(ap);
    exit(EXIT_FAILURE);
}

static const struct option cmdline_options[] = {
    {"help", 0, NULL, 'h'},
    {"port", 0, NULL, 'p'}
};

static _Noreturn void help(const char *pname) {
    printf(_("Usage: %s [OPTIONS]\n\
\n\
Options:\n\
  -p, --port <port> Set the port to use (default: %u).\n\
  -h, --help        Print this help text and exit.\n\
"), pname, DEFAULT_PORT);
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    int port = DEFAULT_PORT;

    int ch;
    while((ch = getopt_long(argc, argv, "hp:", cmdline_options, NULL)) != EOF) {
        switch(ch) {
            case 'h':
                help(argv[0]);
            case 'p':
                port = (int) strtol(optarg, NULL, 10);
                if(errno)
                    panic("%s: option -- 'p' expects numeric argument: %s", argv[0], strerror(errno));
                break;
            default:
                panic(_("%s: invalid option -- '%c'\n\
Try `%s --help` for more information.\n"), argv[0], ch, argv[0]);
        }
    }

    server_t* server = server_init(port);
    if(!server)
        panic("Could not create server.\n");

    if(server_launch(server) != 0)
        return EXIT_FAILURE; 
    server_finish(server); 

    return EXIT_SUCCESS;
}

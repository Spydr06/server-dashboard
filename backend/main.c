#include "server.h"
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

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
    {"root", 0, NULL, 'r'},
    {"help", 0, NULL, 'h'},
    {"port", 0, NULL, 'p'},
    {NULL,   0, NULL, 0  }
};

static _Noreturn void help(const char *pname) {
    printf(_("Usage: %s [OPTIONS]\n\
\n\
Options:\n\
  -r, --root <directory>    Set the server files root directory.\n\
  -p, --port <port>         Set the port to use (default: %u).\n\
  -h, --help                Print this help text and exit.\n\
"), pname, DEFAULT_PORT);
    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    int port = DEFAULT_PORT;
    char* root = ".";

    int ch;
    while((ch = getopt_long(argc, argv, "hp:r:", cmdline_options, NULL)) != EOF) {
        switch(ch) {
            case 'h':
                help(argv[0]);
            case 'r':
                root = strdup(optarg);           
                break;
            case 'p':
                port = (int) strtol(optarg, NULL, 10);
                if(errno)
                    panic("%s: option -- 'p' expects numeric argument: %s", argv[0], strerror(errno));
                break;
            case '?':
            default:
                panic(_("%s: invalid option -- '%c'\n\
Try `%s --help` for more information.\n"), argv[0], ch, argv[0]);
        }
    }

    server_t* server = server_init(port, root);
    if(!server)
        panic("Could not create server.\n");

    pthread_t thread;
    if((thread = server_launch(server)) == 0)
        goto finish;


    sigset_t sigset, original;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);

    int ret;
    if((ret = sigprocmask(SIG_BLOCK, &sigset, &original)) == -1) {
        fprintf(stderr, "Error blocking SIGINT: %s\n", strerror(errno));
        goto finish;
    }

    int signum;
    if((ret = sigwait(&sigset, &signum)) == -1) {
        fprintf(stderr, "Error waiting for SIGINT: %s\n", strerror(errno));
        goto finish;
    }

finish:
    if(server)
        server_finish(server, thread); 
    if(strcmp(root, ".") != 0)
        free(root);
    return ret < 0;
}


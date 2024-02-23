#ifndef RESOURCES_H
#define RESOURCES_H

#include <stddef.h>
#include <stdint.h>

#include <sys/utsname.h>
#include <sys/statvfs.h>

typedef struct {
    struct {
        uint64_t free;
        uint64_t total;
    } memory;
    struct {
        uint64_t free;
        uint64_t total;
    } swap;
    struct {
        uint8_t num_cpus;
        double usage;
    } cpu;
    struct statvfs rootfs;
    struct utsname uname;
} resources_t;

int fetch_resources(resources_t* resources);

#ifndef NDEBUG
void dbg_print_resources(resources_t* resources);
#endif

#endif /* RESOURCES_H */


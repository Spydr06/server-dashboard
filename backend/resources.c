#include "resources.h"

#include <string.h>
#include <errno.h>

#include <sys/statvfs.h>
#include <sys/utsname.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <fcntl.h>

#define _(str) str

#define GB (1024 * 1024 * 1024)

static double fetch_cpu_usage(void) {
    static uint64_t prev_idle_time = 0, prev_total_time = 0;

    int fd = open("/proc/stat", O_RDONLY);
    if(!fd)
        return -1.0;
    
    char buffer[128];
    size_t bytes_read = read(fd, buffer, sizeof buffer - 1);
    buffer[bytes_read] = '\0';
    close(fd);

    uint64_t user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    sscanf(buffer, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);

    printf("%s\n", buffer); 
    printf("%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n", user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice);

    uint64_t total_time = user + nice + system + idle + irq + softirq + steal + guest + guest_nice;
    uint64_t idle_time = idle + iowait;

    uint64_t delta_total_time = total_time - prev_total_time;
    uint64_t delta_idle_time = total_time - prev_idle_time;

    printf("%lu, %lu\n", delta_total_time, delta_idle_time);

    double usage = 100.0 * (1.0 - ((double) delta_idle_time) / (double) delta_total_time);

    prev_total_time = total_time;
    prev_idle_time = idle_time;

    return usage;
}

int fetch_resources(resources_t* resources) {
    struct sysinfo info;    
    if(sysinfo(&info) == -1) {
        fprintf(stderr, "Error: getting sysinfo(): %s\n", strerror(errno));
        return -1;
    }

    resources->memory.total = info.totalram;
    resources->memory.free = info.freeram;
    resources->swap.total = info.totalswap;
    resources->swap.free = info.freeswap;

    resources->cpu.num_cpus = get_nprocs();
    resources->cpu.usage = fetch_cpu_usage();


    uname(&resources->uname);
    statvfs("/", &resources->rootfs);

    return 0;
}

#ifndef NDEBUG
void dbg_print_resources(resources_t* resources) {
    printf(_("System resources:\n\
ram:\n  free: %.2f gb\n  total: %.2f gb\n\
cpu (%d):\n  usage: %.2f\n\
uname: %s\n\
rootfs:\n  free: %.2f gb\n  total: %.2f gb\n\
"), 
        (double) resources->memory.free / (double) GB,
        (double) resources->memory.total / (double) GB,
        resources->cpu.num_cpus,
        resources->cpu.usage,
        resources->uname.release,
        (double) (resources->rootfs.f_bfree * resources->rootfs.f_frsize) / GB,
        (double) (resources->rootfs.f_blocks * resources->rootfs.f_frsize) / GB
    );
}
#endif



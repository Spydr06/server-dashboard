#ifndef _API_H
#define _API_H

#define STATUS_REQUEST_ENDPOINT "/api/status"

#include <stdint.h>
#include <stdbool.h>

typedef struct service_status {
    const char service_name[32];
    uint32_t cpu_time;
    uint64_t memory : 63; 
    bool active     : 1; 
} __attribute__((packed)) service_status_t;

typedef struct api_request_status {
    uint32_t ping; // ms

    float cpu_usage; // %
    
    uint64_t disk_size; // 1
    uint64_t disk_usage; // B
    
    uint64_t total_memory; // B
    uint64_t memory_usage; // B
    uint64_t total_swap; // B
    uint64_t swap_usage; // B
} api_request_status_t;

#endif


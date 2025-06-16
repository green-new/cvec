#include "c_log.h"

#include <stdio.h>
#include <time.h>

// TODO - not thread-safe
static FILE* out = NULL;

void
G_Log(const char* tag, const char* msg) {
    // open the file if not opened
    if (!out) {
        out = fopen("log.out", "w");
    }
    // get time
    time_t now;
    time(&now);
    struct tm* tm_info = localtime(&now);

    char time_str[64];
    strftime(time_str, sizeof(time_str), "%a %b %d %H:%M:%S %Y", tm_info);

    #define LOG_MAX_MESSAGE_LENGTH 1024

    char log_message[LOG_MAX_MESSAGE_LENGTH] = { 0 };

    snprintf(
        log_message,
        LOG_MAX_MESSAGE_LENGTH,
        "%s [%s]: %s\n", 
        time_str, 
        tag, 
        msg
    );
    
    // log the messages to stdout and file
    printf("%s", log_message);
    if (out) {
        fprintf(out, "%s", log_message);
        fflush(out);
    }
}
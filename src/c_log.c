#include "c_log.h"

#include <stdio.h>
#include <time.h>

// TODO - not thread-safe
static FILE* out;

void
G_Log(const char* tag, const char* msg) {
    // open the file if not opened
    if (!out) {
        fopen("log.out", "w");
    }
    // get time
    time_t now;
    time(&now);

    #define LOG_MAX_MESSAGE_LENGTH 1024

    char log_message[LOG_MAX_MESSAGE_LENGTH] = { 0 };

    snprintf(
        log_message,
        LOG_MAX_MESSAGE_LENGTH,
        "%s [%s]: %s", 
        ctime(&now), 
        tag, 
        msg
    );
    
    // log the messages to stdout and file
    printf("%s", log_message);
    fprintf(out, "%s", log_message);
}
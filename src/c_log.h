#ifndef LOG_H_
#define LOG_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/**
 * Print a log message to file and stdout.
 */
void
G_Log(const char* tag, const char* msg);

#endif
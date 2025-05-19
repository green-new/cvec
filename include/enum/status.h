#ifndef ERROR_H__
#define ERROR_H__

typedef enum StatusCode {
    SUCCESS = 0,
    ABORTED = 1,
    MEMORY_REFUSED = 2,
    PARSING_FAILURE = 3,
    DIVISION_BY_ZERO = 4,
    INTEGER_OVERFLOW = 5,
    BUFFER_OVERRUN = 6
} StatusCode;

#endif
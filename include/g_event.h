#ifndef EVENT_H_
#define EVENT_H_

#include "time.h"

typedef enum event_type {
    EVENT_INIT
} event_type;

typedef struct event_node {
    event_type type;
    clock_t time;
} event_node;

typedef struct event_queue {
    event_node* front;
} event_queue;

#endif
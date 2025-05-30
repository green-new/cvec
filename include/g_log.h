#ifndef LOG_H_
#define LOG_H_

#include "stdlib.h"
#include "stdio.h"

#define LOG_MAX_MESSAGE_LENGTH 512

typedef struct log_node {
    char msg[LOG_MAX_MESSAGE_LENGTH];
    size_t len;
    struct log_node* next;
    struct log_node* back;
} log_node;

typedef struct log {
    log_node* queue;
    log_node* back;
};

void
log_push(log* log, char* msg, size_t len) {
    log_node* node = log_node_create(msg, len);
    if (!log->queue) {
        log->queue = node;
    }
    if (!log->back) {
        node->back = NULL;
        node->next = NULL;
        log->back = node;
    } else {
        log->back->next = node;
        node->back = log->back;
        log->back = node;
    }
}

log_node* 
log_pop(log* log) {
    log_node* node = log->back;

    if (!node) {
        if (node->back) {
            node->back->next = NULL;
        }
        node->back = NULL;
        node->next = NULL;
    }

    return node;
}

void
log_node_destroy(log_node* node) {
    if (!node) {
        return;
    }
    for (unsigned i = 0; i < LOG_MAX_MESSAGE_LENGTH; i++) {
        node->msg[i] = 0;
    }
    node->len = 0;  
    free(node); 
}

log_node*
log_node_create(char* msg, size_t len) {
    log_node* node = (log_node*) calloc(1, sizeof node);
    node->len = len;
    for (size_t i = 0; i < len; i++) {
        node->msg[i] = msg[i];
    }
    node->back = NULL;
    node->next = NULL;
    return node;
}

#endif
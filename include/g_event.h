#ifndef EVENT_H_
#define EVENT_H_

#include <time.h>
#include <stdint.h>
#include <stddef.h>

/** Maximum callbacks for an event type */
#define EVENT_MAX_SUBSCRIBERS 16

/**
 * @brief Event type is just a 64-bit value.
 */
typedef uint64_t event_type;

/**
 * @struct event_payload
 * @brief Contains data that the game copies over to the event for use in the 
 * callback function.
 */
typedef struct event_payload {
    /** The data in the payload. Each element can be of any size, so be 
     * careful. 
     */
    void** data;
    /** The number of items. */
    size_t count;
} event_payload;

/**
 * @struct event
 * @brief Contains data surrounding a single event occuring during runtime
 */
typedef struct event {
    /** The type of event. */
    event_type type;
    /** The CPU time this event was fired. */
    clock_t time;
    /** The data package that comes with this event type. */
    event_payload payload;
} event;

/** 
 * @brief Event callback function signature 
 */
typedef void (*event_callback)(event* evt);

/**
 * @struct event_type_info
 * @brief Stores specific information about an event type.
 * An event type has a name, a description, and a maximum number of 16 
 * callbacks.
 */
typedef struct event_type_info {
    /** Name of the event type. */
    const char name[64];
    /** A short description of the event. */
    const char desc[512];
    /** The list of callbacks to call when this event type is fired. */
    event_callback callbacks[EVENT_MAX_SUBSCRIBERS];
    /** The list of active callbacks for this event type. */
    size_t callback_count;
} event_type_info;

/**
 * @struct event_type_map_pair
 * @brief A pair of event type, event details values.
 */
typedef struct event_type_map_pair {
    /** The type of event. */
    event_type type;
    /** Details around a specific event type. */
    event_type_info info;
} event_type_map_pair;

/**
 * @struct event_type_map_bucket
 * @brief Contains a list of map pairs.
 */
typedef struct event_type_map_bucket {
    /** The list of pairs. */
    event_type_map_pair* pairs;
    /** The number of pairs in this bucket. */
    size_t active;
    /** The number of allowed pairs for this bucket. */
    size_t capacity;
} event_type_map_bucket;

/**
 * @struct event_type_map
 * @brief Contains a map of key, value pairs containing the event type as the
 * key and its details as the value.
 */
typedef struct event_type_map {
    /** Heap allocated contiguous list of buckets addressable by hash code,
     * mod the currenct capacity of the map.
     */
    event_type_map_bucket* buckets;
    /**
     * Maximum capacity of the map at a certain point. Once the number
     * of active buckets reaches the load factor, this capacity is doubled from 
     * its previous value. Default is 16.
     */
    unsigned int capacity;
    /** The number of heap allocated buckets which may or may not contain
     * any pairs.
     */
    unsigned int active;
    /** The number of buckets that, once reached, increases the capacity
     * by double its previous value. Default is 12.
     */
    unsigned int load_factor;
} event_type_map;

/**
 * @struct event_node
 * @brief A node that contains the event and the next one in line.
 */
typedef struct event_node {
    /** Pointer to the next event in the queue. */
    struct event_node* next;
    /** Details of the event. */
    event event;
} event_node;

/**
 * @struct event_queue
 * @brief Contains a queue of events that is continually popped/pushed during
 * runtime.
 */
typedef struct event_queue {
    /** Front of the event queue. */
    event_node* front;
    /** Back of the event queue. */
    event_node* back;
    /** Number of used event nodes containing event details. */
    size_t size;
} event_queue;

/**
 * @brief Inserts an event to the event queue.
 * @param q The event queue.
 * @param evt The event.
 */
void
event_queue_push(event_queue* q, event evt);

event*
event_queue_pop(event_queue* q);

#endif /* EVENT_H_ */
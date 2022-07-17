
#include "tsqueue.h"
#include <pthread.h>
#include <stdlib.h>

struct ts_queue_node {
    void* data;
    struct ts_queue_node* prev;
    struct ts_queue_node* next;
};
__attribute__((always_inline)) static void
node_init(struct ts_queue_node* n, void* data) {
    n->next = NULL;
    n->prev = NULL;
    n->data = data;
}
__attribute__((always_inline)) static struct ts_queue_node*
node_create(void* data) {
    struct ts_queue_node* n = malloc(sizeof(*n));
    node_init(n, data);
    return n;
}

struct ts_queue {
    pthread_mutex_t lock;
    unsigned long length;
    struct ts_queue_node head;
};

struct ts_queue* ts_queue_create() {
    struct ts_queue* q = malloc(sizeof(*q));
    if(!ts_queue_init(q)) {
        free(q);
        return NULL;
    }
    return q;
}
bool ts_queue_init(struct ts_queue* queue) {
    if(pthread_mutex_init(&queue->lock, NULL) != 0) {
        return false;
    }
    queue->length = 0;
    // init dummy
    struct ts_queue_node* head = &queue->head;
    node_init(head, NULL);
    head->next = head;
    head->prev = head;

    return true;
}

__attribute__((always_inline)) static void
ts_queue_enqueue_impl(struct ts_queue* queue, void* data) {
    struct ts_queue_node* elm = node_create(data);
    struct ts_queue_node* head = &queue->head;
    struct ts_queue_node* last = head->prev;

    last->next = elm;
    elm->prev = last;

    elm->next = head;
    head->prev = elm;

    queue->length++;
}
__attribute__((always_inline)) static bool
ts_queue_empty_impl(struct ts_queue* queue) {
    struct ts_queue_node* head = &queue->head;
    return head == head->next;
}
__attribute__((always_inline)) static void*
ts_queue_dequeue_impl(struct ts_queue* queue) {
    if(ts_queue_empty_impl(queue)) return NULL;

    struct ts_queue_node* head = &queue->head;

    struct ts_queue_node* elm = head->next;
    head->next = elm->next;
    elm->next->prev = head;

    void* data = elm->data;
    free(elm);
    queue->length--;
    return data;
}
__attribute__((always_inline)) static void*
ts_queue_peek_impl(struct ts_queue* queue) {
    if(ts_queue_empty_impl(queue)) return NULL;
    struct ts_queue_node* head = &queue->head;
    struct ts_queue_node* elm = head->next;
    return elm->data;
}
__attribute__((always_inline)) static unsigned long
ts_queue_length_impl(struct ts_queue* queue) {
    return queue->length;
}

// #define ENSURE_SEQUENTIAL_ONERROR(lock_, code_, onerror_)                      \
//     do {                                                                       \
//         if(pthread_mutex_lock(lock_) != 0) {                                   \
//             onerror_;                                                          \
//         }                                                                      \
//         code_;                                                                 \
//         if(pthread_mutex_unlock(lock_) != 0) {                                 \
//             onerror_;                                                          \
//         }                                                                      \
//     } while(0)
#define ENSURE_SEQUENTIAL_ONERROR(lock_, code_, onerror_)                      \
    do {                                                                       \
        code_;                                                                 \
    } while(0)
#define C_NOP                                                                  \
    do {                                                                       \
    } while(0)

#define ENSURE_SEQUENTIAL(lock_, code_)                                        \
    ENSURE_SEQUENTIAL_ONERROR(lock_, code_, C_NOP;)

void ts_queue_enqueue(struct ts_queue* queue, void* data) {
    ENSURE_SEQUENTIAL_ONERROR(
        &queue->lock,
        ts_queue_enqueue_impl(queue, data),
        return );
}
void* ts_queue_dequeue(struct ts_queue* queue) {
    void* data;
    ENSURE_SEQUENTIAL_ONERROR(
        &queue->lock,
        data = ts_queue_dequeue_impl(queue),
        return NULL);
    return data;
}
void* ts_queue_peek(struct ts_queue* queue) {
    void* data;
    ENSURE_SEQUENTIAL_ONERROR(
        &queue->lock,
        data = ts_queue_peek_impl(queue),
        return NULL);
    return data;
}
bool ts_queue_empty(struct ts_queue* queue) {
    bool empty;
    ENSURE_SEQUENTIAL_ONERROR(
        &queue->lock,
        empty = ts_queue_empty_impl(queue),
        return false);
    return empty;
}
unsigned long ts_queue_length(struct ts_queue* queue) {
    unsigned long length;
    ENSURE_SEQUENTIAL_ONERROR(
        &queue->lock,
        length = ts_queue_length_impl(queue),
        return -1);
    return length;
}

void ts_queue_destroy(struct ts_queue* queue) {
    struct ts_queue_node* head = &queue->head;
    while(head != head->next) {
        struct ts_queue_node* elm = head->next;
        head->next = elm->next;
        elm->next->prev = head;
        free(elm);
    }
    pthread_mutex_destroy(&queue->lock);
    free(queue);
}

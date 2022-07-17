#include "_queue.h"
#include <stdlib.h>

// init a queue head
struct ptp_queue* queue_create() {
    // the head is a dummy
    struct ptp_queue* head = queue_node_create();
    head->next = head;
    head->prev = head;
    return head;
}

// destroy a queue, freeing all elms
void queue_destroy(struct ptp_queue* head, void (*clean_data)(void*)) {
    while(head != head->next) {
        struct ptp_queue* to_del = head->next;
        head->next = to_del->next;
        to_del->next->prev = head;
        queue_node_destroy(to_del, clean_data);
    }

    // all thats left is the head
    // no destructor needed, its empty
    queue_node_destroy(head, NULL);
}

// init a new node and return
struct ptp_queue* queue_node_create() {
    struct ptp_queue* node = malloc(sizeof(*node));
    node->next = NULL;
    node->prev = NULL;
    node->data = NULL;
    return node;
}

// free a node and set it to null
void queue_node_destroy(struct ptp_queue* node, void (*clean_data)(void*)) {
    if(clean_data) clean_data(node->data);
    free(node);
}

// check if the queue is empty
int queue_is_empty(struct ptp_queue* head) { return head == head->next; }

// to add an item, set the heads prev to be the item
void queue_enqueue(struct ptp_queue* head, void* data) {
    struct ptp_queue* item = queue_node_create();
    item->data = data;

    struct ptp_queue* last = head->prev;

    last->next = item;
    item->prev = last;

    item->next = head;
    head->prev = item;
}
// to remove an item, unlink from the queue and return
void* queue_dequeue(struct ptp_queue* head) {
    if(head != head->next) {
        struct ptp_queue* to_del = head->next;
        head->next = to_del->next;
        to_del->next->prev = head;
        void* data = to_del->data;
        queue_node_destroy(to_del, NULL); // dont free data
        return data;
    }
    return NULL;
}

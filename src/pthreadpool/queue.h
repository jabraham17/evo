
#ifndef PTHREADPOOL_QUEUE_H_
#define PTHREADPOOL_QUEUE_H_

// the heads prev is the most recently added element and the last to come out
// the heads next is the next element
// implemented with a dummy node, the head node is has no payload
// the head node has no next pointer, then the queue is empty
struct ptp_queue {
    void* data;
    struct ptp_queue* next;
    struct ptp_queue* prev;
};
struct ptp_queue* queue_create();
void queue_destroy(struct ptp_queue* head, void (*clean_data)(void*));

struct ptp_queue* queue_node_create();
void queue_node_destroy(struct ptp_queue* node, void (*clean_data)(void*));

int queue_is_empty(struct ptp_queue* head);
void queue_enqueue(struct ptp_queue* head, void* data);
void* queue_dequeue(struct ptp_queue* head);

#endif

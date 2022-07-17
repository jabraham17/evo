#ifndef SIMULATOR_TSQUEUE_TSQUEUE_H_
#define SIMULATOR_TSQUEUE_TSQUEUE_H_

#include <stdbool.h>
struct ts_queue;

struct ts_queue* ts_queue_create();
bool ts_queue_init(struct ts_queue* queue);

void ts_queue_enqueue(struct ts_queue* queue, void* data);
void* ts_queue_dequeue(struct ts_queue* queue);
void* ts_queue_peek(struct ts_queue* queue);
bool ts_queue_empty(struct ts_queue* queue);
unsigned long ts_queue_length(struct ts_queue* queue);

void ts_queue_destroy(struct ts_queue* queue);

#endif


#ifndef PTHREADPOOL_POOL_H_
#define PTHREADPOOL_POOL_H_

struct ptp_pool;
struct ptp_task;

// init a pool and start up the threads
struct ptp_pool* pool_create(unsigned long n_threads);
// destroy and close threads
void pool_destroy(struct ptp_pool* pool);
// submit work to pool
struct ptp_task* pool_submit(
    struct ptp_pool* pool,
    void* (*function)(void*),
    void* argument,
    void** result);
// wait for a specific work to complete
void pool_wait(struct ptp_task* task);

unsigned long pool_get_num_threads(struct ptp_pool* pool);

#endif

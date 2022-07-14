#include "pool.h"

#include "queue.h"
#include <pthread.h>
#include <stdlib.h>

struct ptp_pool {
    // thread management
    pthread_t* threads;
    unsigned long n_threads;

    // queue management
    struct ptp_queue* queue;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_signal;
};

struct ptp_task {
    void* argument;
    void* (*function)(void*);
    void** result;
    unsigned int done;
    pthread_cond_t done_signal;
    pthread_mutex_t done_lock;
};

typedef struct worker_cleanup_arg_t {
    pthread_mutex_t* lock;
} worker_cleanup_arg_t;
// cleanup worker when it is canceld
// do this by destroying the node and unlocking the mutex
static void worker_cleanup(void* arg) {
    worker_cleanup_arg_t* cleanup_arg = (worker_cleanup_arg_t*)arg;
    // unlock the queue
    pthread_mutex_unlock(cleanup_arg->lock);
}

static void* worker(void* arg) {
    int oldstate;
    struct ptp_pool* pool = (struct ptp_pool*)arg;

    while(1) {
        // we can cancel anytime in the aquisition of a new task
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
        // check if we are canceled
        pthread_testcancel();

        // lock the queue
        pthread_mutex_lock(&pool->queue_lock);

        // try and get the next task, if empty wait for a signal
        struct ptp_task* task = NULL;

        // setup cancel handler
        worker_cleanup_arg_t cleanup_arg = {.lock = &pool->queue_lock};
        pthread_cleanup_push(worker_cleanup, (void*)(&cleanup_arg));

        while((task = (struct ptp_task*)queue_dequeue(pool->queue)) == NULL) {
            pthread_cond_wait(&pool->queue_signal, &pool->queue_lock);
        }
        // non-zero value to execute cleanup
        pthread_cleanup_pop(1);

        // check if we are canceled
        pthread_testcancel();

        // we have a task, cannot cancel now
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);

        // we should have a valid task now, complete the task
        // if there is a result, use it, otherwise dont
        if(task->result) *(task->result) = task->function(task->argument);
        else task->function(task->argument);

        pthread_mutex_lock(&task->done_lock);
        task->done = 1;
        // task is complete, signal
        pthread_cond_signal(&task->done_signal);
        pthread_mutex_unlock(&task->done_lock);
    }
    return NULL;
}

// init a pool and start up the threads
struct ptp_pool* pool_create(unsigned long n_threads) {
    int ret;
    struct ptp_pool* pool = malloc(sizeof(*pool));
    pool->n_threads = n_threads;

    // allocate the queue
    pool->queue = queue_create();
    // init mutex, if fail free resources and return NULL
    ret = pthread_mutex_init(&pool->queue_lock, NULL);
    if(ret != 0) {
        queue_destroy(pool->queue, NULL);
        free(pool);
        return NULL;
    }
    // init cond, if fail free resources and return NULL
    ret = pthread_cond_init(&(pool->queue_signal), NULL);
    if(ret != 0) {
        // should never be non zero
        ret = pthread_mutex_destroy(&pool->queue_lock);
        queue_destroy(pool->queue, NULL);
        free(pool);
        return NULL;
    }

    // alloc threads
    pool->threads = malloc(pool->n_threads * sizeof(*pool->threads));
    // create the threads
    for(unsigned int i = 0; i < pool->n_threads; i++) {
        ret = pthread_create(&pool->threads[i], NULL, worker, (void*)pool);
    }

    return pool;
}

// destroy the task
static void pool_task_destroy(struct ptp_task* task) {
    pthread_cond_destroy(&task->done_signal);
    pthread_mutex_destroy(&task->done_lock);
    free(task);
}
static void task_destructor(void* arg) {
    if(arg == NULL) return;
    struct ptp_task* data = (struct ptp_task*)arg;
    pool_task_destroy(data);
}

// destroy and close threads
void pool_destroy(struct ptp_pool* pool) {
    // cancel all threads
    for(unsigned long i = 0; i < pool->n_threads; i++) {
        pthread_cancel(pool->threads[i]);
    }

    // join the threads
    for(unsigned long i = 0; i < pool->n_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    // cleanup the threads array
    free(pool->threads);

    // cleanup the mutexs
    pthread_cond_destroy(&pool->queue_signal);
    pthread_mutex_destroy(&pool->queue_lock);

    // destroy remaining tasks
    queue_destroy(pool->queue, task_destructor);

    // free the pool
    free(pool);
}

// submit work to pool
struct ptp_task* pool_submit(
    struct ptp_pool* pool,
    void* (*function)(void*),
    void* argument,
    void** result) {
    int ret;
    // init basic task items
    struct ptp_task* task = malloc(sizeof(*task));
    task->argument = argument;
    task->result = result;
    task->function = function;
    task->done = 0; // not done

    // init the signal
    ret = pthread_mutex_init(&task->done_lock, NULL);
    if(ret != 0) {
        free(task);
        return NULL;
    }
    ret = pthread_cond_init(&task->done_signal, NULL);
    if(ret != 0) {
        // should never be non zero
        ret = pthread_mutex_destroy(&task->done_lock);
        free(task);
        return NULL;
    }

    // submit task to pool
    // lock the queue
    ret = pthread_mutex_lock(&pool->queue_lock);
    // init and add
    queue_enqueue(pool->queue, (void*)task);

    // signal we have a new task
    ret = pthread_cond_signal(&pool->queue_signal);

    // unlock
    ret = pthread_mutex_unlock(&pool->queue_lock);

    return task;
}

// wait for a specific work to complete
void pool_wait(struct ptp_task* task) {
    // while not done, wait for signal
    pthread_mutex_lock(&task->done_lock);
    while(!task->done) {
        pthread_cond_wait(&task->done_signal, &task->done_lock);
    }
    pthread_mutex_unlock(&task->done_lock);

    // wait is done, we can destroy the task
    pool_task_destroy(task);
}

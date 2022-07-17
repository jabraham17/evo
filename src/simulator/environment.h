#ifndef SIMULATOR_ENVIRONMENT_H_
#define SIMULATOR_ENVIRONMENT_H_

#include "creature.h"
#include <stddef.h>
#include <stdint.h>

#if defined(THREADED) && THREADED == 1
    #include "pthreadpool/pool.h"
#endif

struct environment_args {
    long seed;

    char* output_dir;
    size_t output_scale;
    int callback_tick;
    int callback_start;
    int callback_end;
    int callback_select;
    size_t callback_tick_freq;

    size_t width;
    size_t height;
    size_t n_creatures;
    size_t n_generations;
    size_t n_ticks;
    int threshold;
    size_t n_connections;
    float mutation_rate;

#if defined(THREADED) && THREADED == 1
    size_t n_threads;
#endif
};

// a feature of this environment, all the memory is together
// there are 2 dynamic allocations, the creatures and the grid
// the creatures are all allocated in one go, meaning the memory is contiguous
struct environment {
    size_t width;
    size_t height;
    struct creature* creatures;
    size_t n_creatures;
    struct creature** grid; // pointers to `creatures` array
#if defined(THREADED) && THREADED == 1
    struct ptp_pool* thread_pool;
#endif
    struct environment_args* args;
};

enum grid_state {
    S_NONE = 0x0,
    S_WALL_LEFT = 0x1,
    S_WALL_RIGHT = 0x2,
    S_WALL_UP = 0x4,
    S_WALL_DOWN = 0x8,
    S_CREATURE_LEFT = 0x10,
    S_CREATURE_RIGHT = 0x20,
    S_CREATURE_UP = 0x40,
    S_CREATURE_DOWN = 0x80,
    S_HAS_CREATURE = 0x100
};
typedef uint32_t grid_state_t;
_Static_assert(sizeof(enum grid_state) == sizeof(grid_state_t), "");

enum selection_criteria { SELECTION_LEFT, SELECTION_RIGHT, SELECTION_CENTER, SELECTION_BR_CORNER, SELECTION_LONELY };

struct environment_callback_data {
    struct environment* env;
    size_t generation;
    size_t tick;
};

typedef void (*environment_callback_t)(struct environment_callback_data*);

struct environment* environment_create(struct environment_args*);
void environment_add_creatures(struct environment*, size_t n_creatures);
void environment_subtract_creatures(struct environment*, size_t n_creatures);
void environment_set_creatures(struct environment*, size_t n_creatures);

grid_state_t
environment_get_grid_state(struct environment* env, size_t grid_idx);

void environment_run_simulation(
    struct environment*,
    environment_callback_t generation_start_callback,
    environment_callback_t microtick_callback,
    environment_callback_t generation_end_callback,
    environment_callback_t generation_select_callback);

void environment_run_generation(
    struct environment*,
    size_t generation,
    environment_callback_t callback);
void environment_next_generation(struct environment*);
void environment_microtick(struct environment*);

void environment_select(
    struct environment*,
    enum selection_criteria selection_criteria);
// mutation invalidates all locations!!!!
void environment_mutate(struct environment*);
void environment_distribute(struct environment*);

bool environment_move_creature(
    struct environment*,
    size_t grid_idx_src,
    size_t grid_idx_dst);

size_t environment_number_alive(struct environment*);
void environment_creature_consolidate(struct environment*);

void environment_destroy(struct environment*);

#endif


#include "environment.h"
#include "common/common.h"
#include "tsqueue/tsqueue.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// private
#define PRIV_FN __attribute__((unused)) static

PRIV_FN bool creature_has_survived(
    struct environment* env,
    size_t grid_idx,
    enum selection_criteria selection_criteria) {
    point_t location = {
        .x = common_get_col(grid_idx, env->width),
        .y = common_get_row(grid_idx, env->width)};

    if(creature_is_dead(env->grid[grid_idx])) return false;

    switch(selection_criteria) {
        case SELECTION_LEFT: return location.x < (env->width / 2);
        default: return true;
    }
}

PRIV_FN void reset_grid(struct environment* env) {
    memset(env->grid, 0, env->width * env->height * sizeof(*env->grid));
}

PRIV_FN size_t find_last_alive_creature(struct environment* env) {
    for(size_t i = env->n_creatures - 1; i != (size_t)-1; i--) {
        if(creature_is_alive(&env->creatures[i])) return i;
    }
    return env->n_creatures; // return size if none found
}

#undef PRIV_FN

// public
struct environment* environment_create(struct environment_args* args) {
    struct environment* env = malloc(sizeof(*env));
    env->width = args->width;
    env->height = args->height;
    env->creatures = NULL;
    env->n_creatures = 0;
    env->grid = calloc(env->width * env->height, sizeof(*env->grid));
    env->args = args;

#if defined(THREADED) && THREADED == 1
    env->thread_pool = pool_create(args->n_threads);
#endif

    environment_add_creatures(env, args->n_creatures);
    for(size_t i = 0; i < env->n_creatures; i++) {
        creature_init(&env->creatures[i], SPECIES_A, env->args->n_connections);
    }
    environment_distribute(env);

    return env;
}

void environment_add_creatures(struct environment* env, size_t n_creatures) {
    // start adding new creatures at the end of the old creature array
    size_t starting = env->n_creatures;
    env->n_creatures += n_creatures;
    if(env->creatures == NULL)
        env->creatures = malloc(env->n_creatures * sizeof(*env->creatures));
    else
        env->creatures =
            realloc(env->creatures, env->n_creatures * sizeof(*env->creatures));

    // zero new creartures
    memset(
        &env->creatures[starting],
        0,
        (env->n_creatures - starting) * sizeof(*env->creatures));
}

void environment_subtract_creatures(
    struct environment* env,
    size_t n_creatures) {

    if(env->n_creatures > n_creatures) {
        env->n_creatures -= n_creatures;
        if(env->creatures == NULL)
            env->creatures = malloc(env->n_creatures * sizeof(*env->creatures));
        else
            env->creatures = realloc(
                env->creatures,
                env->n_creatures * sizeof(*env->creatures));
    } else {
        env->n_creatures = 0;
        free(env->creatures);
        env->creatures = NULL;
    }
}

void environment_set_creatures(struct environment* env, size_t n_creatures) {

    if(env->n_creatures == n_creatures) return;
    else if(env->n_creatures > n_creatures) {
        environment_subtract_creatures(env, env->n_creatures - n_creatures);
    } else if(env->n_creatures < n_creatures) {
        environment_add_creatures(env, n_creatures - env->n_creatures);
    }
}

grid_state_t
environment_get_grid_state(struct environment* env, size_t grid_idx) {
    point_t location = {
        .x = common_get_col(grid_idx, env->width),
        .y = common_get_row(grid_idx, env->width)};
    grid_state_t state = S_NONE;
    if(creature_is_alive(env->grid[grid_idx])) state |= S_HAS_CREATURE;
    if(location.x == 0) state |= S_WALL_LEFT;
    if(location.y == 0) state |= S_WALL_UP;
    if(location.x == env->width - 1) state |= S_WALL_RIGHT;
    if(location.y == env->height - 1) state |= S_WALL_DOWN;

    size_t idx;
    struct creature* c;

    if(!(state & S_WALL_LEFT)) {
        idx = common_get_idx(location.x - 1, location.y, env->width);
        c = env->grid[idx];
        if(creature_is_alive(c)) state |= S_CREATURE_LEFT;
    }
    if(!(state & S_WALL_RIGHT)) {
        idx = common_get_idx(location.x + 1, location.y, env->width);
        c = env->grid[idx];
        if(creature_is_alive(c)) state |= S_CREATURE_RIGHT;
    }
    if(!(state & S_WALL_UP)) {
        idx = common_get_idx(location.x, location.y - 1, env->width);
        c = env->grid[idx];
        if(creature_is_alive(c)) state |= S_CREATURE_UP;
    }
    if(!(state & S_WALL_DOWN)) {
        idx = common_get_idx(location.x, location.y + 1, env->width);
        c = env->grid[idx];
        if(creature_is_alive(c)) state |= S_CREATURE_DOWN;
    }

    return state;
}

void environment_run_simulation(
    struct environment* env,
    environment_callback_t generation_start_callback,
    environment_callback_t microtick_callback,
    environment_callback_t generation_end_callback,
    environment_callback_t generation_select_callback) {
    struct environment_callback_data d;
    d.env = env;

    for(size_t i = 1; i <= env->args->n_generations; i++) {
        d.generation = i;
        d.tick = 0;
        if(env->args->callback_start && generation_start_callback)
            generation_start_callback(&d);
        environment_run_generation(env, i, microtick_callback);
        if(env->args->callback_end && generation_end_callback)
            generation_end_callback(&d);

        environment_select(env, SELECTION_LEFT);
        if(env->args->callback_select && generation_select_callback)
            generation_select_callback(&d);

        environment_next_generation(env);
    }
}

void environment_run_generation(
    struct environment* env,
    size_t generation,
    environment_callback_t callback) {
    struct environment_callback_data d;
    d.env = env;
    d.generation = generation;
    for(size_t i = 1; i <= env->args->n_ticks; i++) {
        environment_microtick(env);

        d.tick = i;
        if(env->args->callback_tick &&
           (i % env->args->callback_tick_freq == 0) && callback)
            callback(&d);
    }
}

void environment_next_generation(struct environment* env) {
    environment_mutate(env);
    environment_distribute(env);
}

#if defined(THREADED) && THREADED == 1
struct environment_microtick_thread_arg {
    size_t start;
    size_t stop;
    struct environment* env;
    struct ts_queue* workqueue;
};
static void* environment_microtick_thread(void* varg) {
    struct environment_microtick_thread_arg* arg =
        (struct environment_microtick_thread_arg*)varg;
    for(size_t grid_idx = arg->start; grid_idx < arg->stop; grid_idx++) {
        struct creature* creature = arg->env->grid[grid_idx];
        if(creature) {
            grid_state_t state = environment_get_grid_state(arg->env, grid_idx);
            creature_tick(creature, arg->env, grid_idx, state, arg->workqueue);
        }
    }
    return NULL;
}
void environment_microtick(struct environment* env) {
    size_t grid_size = env->width * env->height;
    size_t step = grid_size / 4;
    struct environment_microtick_thread_arg args[4];
    args[0].start = 0;
    args[0].stop = step;
    args[0].env = env;
    args[0].workqueue = ts_queue_create();
    args[1].start = args[0].stop;
    args[1].stop = args[0].stop + step;
    args[1].env = env;
    args[1].workqueue = ts_queue_create();
    args[2].start = args[1].stop;
    args[2].stop = args[1].stop + step;
    args[2].env = env;
    args[2].workqueue = ts_queue_create();
    args[3].start = args[2].stop;
    args[3].stop = grid_size;
    args[3].env = env;
    args[3].workqueue = ts_queue_create();

    struct ptp_task* t0 =
        pool_submit(env->thread_pool, environment_microtick_thread, &args[0], NULL);
    struct ptp_task* t1 =
        pool_submit(env->thread_pool, environment_microtick_thread, &args[1], NULL);
    struct ptp_task* t2 =
        pool_submit(env->thread_pool, environment_microtick_thread, &args[2], NULL);
    struct ptp_task* t3 =
        pool_submit(env->thread_pool, environment_microtick_thread, &args[3], NULL);

        #define empty_it(idx) \
    while(!ts_queue_empty(args[idx].workqueue)) { \
        struct creature_workqueue_elm* elm = \
            ts_queue_dequeue(args[idx].workqueue); \
        creature_apply_action( \
            elm->creature, \
            elm->env, \
            elm->grid_idx, \
            elm->action); \
        free(elm); \
    } \
    ts_queue_destroy(args[idx].workqueue);

    pool_wait(t0);
    pool_wait(t1);
    pool_wait(t2);
    pool_wait(t3);

    empty_it(0)
    empty_it(1)
    empty_it(2)
    empty_it(3)
}
#else
void environment_microtick(struct environment* env) {
    size_t grid_size = env->width * env->height;
    for(size_t grid_idx = 0; grid_idx < grid_size; grid_idx++) {
        struct creature* creature = env->grid[grid_idx];
        if(creature) {
            grid_state_t state = environment_get_grid_state(env, grid_idx);
            creature_tick(creature, env, grid_idx, state);
        }
    }
}
#endif

void environment_select(
    struct environment* env,
    enum selection_criteria selection_criteria) {
    for(size_t grid_idx = 0; grid_idx < env->width * env->height; grid_idx++) {
        if(!creature_has_survived(env, grid_idx, selection_criteria)) {
            creature_kill(env->grid[grid_idx]);
        }
    }
}
void environment_mutate(struct environment* env) {
    reset_grid(env);

    environment_creature_consolidate(env);

    size_t n_alive = environment_number_alive(env);
    size_t n_create = clampq(n_alive * 2, 0, env->args->n_creatures);
    environment_set_creatures(env, n_create);

    // go through the front half of the array, copy the creature, then mutate
    // both
    for(size_t i = 0; i < n_alive; i++) {
        struct creature* creature1 = &(env->creatures[i]);

        if(i + n_alive < env->n_creatures) {
            struct creature* creature2 = &(env->creatures[n_alive + i]);
            memmove(creature2, creature1, sizeof(struct creature));
            creature_mutate(creature2, env->args->mutation_rate);
        }
        creature_mutate(creature1, env->args->mutation_rate);
    }
}

void environment_distribute(struct environment* env) {
    for(size_t creature_idx = 0; creature_idx < env->n_creatures;
        creature_idx++) {
        // if there is a creature, generate a location
        struct creature* creature = &env->creatures[creature_idx];
        if(creature_is_alive(creature)) {
            while(1) {
                size_t grid_idx = rand_max(env->width * env->height);
                // if there is already a creature there, keep generating
                if(creature_is_alive(env->grid[grid_idx])) continue;
                env->grid[grid_idx] = creature;
                break;
            }
        }
    }
}

bool environment_move_creature(
    struct environment* env,
    size_t grid_idx_src,
    size_t grid_idx_dst) {
    if(creature_is_dead(env->grid[grid_idx_src])) return false;
    if(creature_is_alive(env->grid[grid_idx_dst])) return false;

    env->grid[grid_idx_dst] = env->grid[grid_idx_src];
    // src is no longer valid
    env->grid[grid_idx_src] = NULL;

    return true;
}

size_t environment_number_alive(struct environment* env) {
    size_t n_alive = 0;
    for(size_t i = 0; i < env->n_creatures; i++)
        if(creature_is_alive(&env->creatures[i])) n_alive++;
    return n_alive;
}

// invalidates all locations!!!!
void environment_creature_consolidate(struct environment* env) {
    // reorder creature array so all alive creatures are at the front
    // size_t last_alive = env->n_creatures - 1;
    for(size_t creature_idx = 0; creature_idx < env->n_creatures;
        creature_idx++) {
        if(creature_is_dead(&env->creatures[creature_idx])) {
            size_t last_alive = find_last_alive_creature(env);
            // if no more alive, break
            if(last_alive == env->n_creatures || last_alive < creature_idx)
                break;
            memmove(
                &env->creatures[creature_idx],
                &env->creatures[last_alive],
                sizeof(struct creature));
            memset(&env->creatures[last_alive], 0, sizeof(struct creature));
        }
    }
}

void environment_destroy(struct environment* env) {
    free(env->creatures);
    free(env->grid);

#if defined(THREADED) && THREADED == 1
    pool_destroy(env->thread_pool);
#endif

    free(env);
}

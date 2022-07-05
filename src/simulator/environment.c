
#include "environment.h"
#include "common/common.h"
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
    memset(*env->grid, 0, env->width * env->height * sizeof(env->grid));
}

PRIV_FN size_t find_next_dead_creature(struct environment* env, size_t start) {
    for(size_t i = start; i < env->n_creatures; i++)
        if(creature_is_dead(&env->creatures[i])) return i;
    return env->n_creatures; // return size if none found
}
PRIV_FN size_t find_next_alive_creature(struct environment* env, size_t start) {
    for(size_t i = start; i < env->n_creatures; i++)
        if(creature_is_alive(&env->creatures[i])) return i;
    return env->n_creatures; // return size if none found
}

#undef PRIV_FN

// public
struct environment* environment_create(size_t width, size_t height) {
    struct environment* env = malloc(sizeof(*env));
    env->width = width;
    env->height = height;
    env->creatures = NULL;
    env->n_creatures = 0;

    env->grid = calloc(env->width * env->height, sizeof(*env->grid));

    return env;
}

void environment_add_creatures(struct environment* env, size_t n_creatures) {
    reset_grid(env);

    // start adding new creatures at the end of the old creature array
    size_t starting = env->n_creatures;
    env->n_creatures += n_creatures;
    if(env->creatures == NULL)
        env->creatures = malloc(env->n_creatures * sizeof(*env->creatures));
    else
        env->creatures =
            realloc(env->creatures, env->n_creatures * sizeof(*env->creatures));

    for(size_t i = starting; i < env->n_creatures; i++) {
        // TODO MAKE SPECIES and vars PARAMETERIZED
        creature_init(&env->creatures[i], SPECIES_A, 10);
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

void environment_run_generation(
    struct environment* env,
    size_t microcount,
    int8_t threshold) {
    for(size_t i = 0; i < microcount; i++) {
        environment_microtick(env, threshold);
    }
}

void environment_next_generation(struct environment* env) {
    environment_mutate(env);
    environment_distribute(env);
}

void environment_microtick(struct environment* env, int8_t threshold) {
    for(size_t i = 0; i < env->n_creatures; i++) {
        creature_tick(&env->creatures[i], env, threshold);
    }
}

void environment_select(
    struct environment* env,
    enum selection_criteria selection_criteria) {
    for(size_t grid_idx = 0; grid_idx < env->width * env->height; grid_idx++) {
        if(!creature_has_survived(env, grid_idx, selection_criteria)) {
            creature_kill(env->grid[grid_idx]);
        }
    }
}
// invalidates all locations!!!!
void environment_mutate(struct environment* env) {
    reset_grid(env);

    environment_creature_consolidate(env);
    size_t n_alive = environment_number_alive(env);

    // this will add n_alive creatures to the existing n_alive creatures
    // (double the creatures)
    environment_add_creatures(env, n_alive);

    // go through the front half of the array, copy the creature, then mutate
    // both
    for(size_t i = 0; i < n_alive; i++) {
        struct creature* creature1 = &env->creatures[i];
        struct creature* creature2 = &env->creatures[n_alive + i];
        memmove(creature2, creature1, sizeof(*creature1));
        // 0.1% chance to mutate
        creature_mutate(creature1, 10);
        creature_mutate(creature2, 10);
    }
}

// FIXME
void environment_distribute(struct environment* env) {
    for(size_t creature_idx = 0; creature_idx < env->n_creatures;
        creature_idx++) {
        // if there is a creature, generate a location
        struct creature* creature = &env->creatures[creature_idx];
        if(creature_is_alive(creature))
            while(1) {
                signed long long grid_idx = rand_max(env->width * env->height);
                // if there is already a creature there, keep generating
                if(creature_is_alive(env->grid[grid_idx])) continue;
                env->grid[grid_idx] = creature;
                break;
            }
    }
}

size_t environment_number_alive(struct environment* env) {
    size_t n_alive = 0;
    for(size_t i = 0; i < env->n_creatures; i++)
        if(creature_is_alive(&env->creatures[i])) n_alive++;
    return n_alive;
}

// invalidates all locations!!!!
void environment_creature_consolidate(struct environment* env) {
    reset_grid(env);
    // reorder creature array so all alive creatures are at the front
    size_t next_alive = 0;
    for(size_t creature_idx = 0; creature_idx < env->width * env->height;
        creature_idx++) {
        if(creature_is_dead(&env->creatures[creature_idx])) {
            next_alive = find_next_alive_creature(env, creature_idx + 1);
            // if no more alive, break
            if(next_alive >= env->n_creatures) break;
            memmove(
                &env->creatures[creature_idx],
                &env->creatures[next_alive],
                sizeof(*env->creatures));
        }
    }
}

void environment_destroy(struct environment* env) {
    free(env->creatures);
    free(env->grid);
    free(env);
}

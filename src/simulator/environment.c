
#include "environment.h"
#include <stdbool.h>
#include <stdlib.h>

// private
static bool creature_has_survived(
    struct environment* env, struct creature* creature,
    enum selection_criteria selection_criteria) {
    switch(selection_criteria) {
        case SELECTION_LEFT: return creature->location.x < (env->width / 2);
        default: return true;
    }
}

// public
struct environment* environment_create(size_t width, size_t height) {
    struct environment* env = malloc(sizeof(*env));
    env->width = width;
    env->height = height;
    env->creatures = NULL;
    env->n_creatures = 0;
    return env;
}

void environment_populate(struct environment* env, size_t n_creatures) {
    env->n_creatures = n_creatures;
    env->creatures = malloc(env->n_creatures * sizeof(env->creatures));
    for(size_t i = 0; i < env->n_creatures; i++) {
        // TODO MAKE PARAMETERIZED
        env->creatures[i] = creature_create(SPECIES_A, 10, 4, 4);
    }
    environment_distribute(env);
}

void environment_get_surroundings(
    struct environment* env, struct creature* creature,
    enum surroundings* surroundings) {
    point_t location = creature->location;
    *surroundings = S_NONE;
    if(location.x == 0) *surroundings |= S_WALL_LEFT;
    if(location.y == 0) *surroundings |= S_WALL_UP;
    if(location.x == env->width - 1) *surroundings |= S_WALL_RIGHT;
    if(location.y == env->height - 1) *surroundings |= S_WALL_DOWN;

    for(size_t i = 0; i < env->n_creatures; i++) {
        // bail out for surrounded on all sides
        // if(surroundings & (S_CREATURE_LEFT | S_CREATURE_RIGHT | S_CREATURE_UP
        // | S_CREATURE_DOWN)) break;
        if(creature_left(creature, env->creatures[i]))
            *surroundings |= S_CREATURE_LEFT;
        if(creature_right(creature, env->creatures[i]))
            *surroundings |= S_CREATURE_RIGHT;
        if(creature_up(creature, env->creatures[i]))
            *surroundings |= S_CREATURE_UP;
        if(creature_down(creature, env->creatures[i]))
            *surroundings |= S_CREATURE_DOWN;
    }
}

void environment_run_generation(
    struct environment* env, size_t microcount, int8_t threshold) {
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
        creature_tick(env->creatures[i], env, threshold);
    }
}

void environment_select(
    struct environment* env, enum selection_criteria selection_criteria) {
    for(size_t i = 0; i < env->n_creatures; i++) {
        if(!creature_has_survived(env, env->creatures[i], selection_criteria)) {
            creature_destroy(env->creatures[i]);
            env->creatures[i] = NULL;
        }
    }
}
// mutation invalidates all locations!!!!
void environment_mutate(struct environment* env) {
    // count old creatures
    size_t n_creatures = 0;
    for(size_t i = 0; i < env->n_creatures; i++)
        if(env->creatures[i]) n_creatures++;

    // we are going to double the creatures
    n_creatures *= 2;
    struct creature** creatures = malloc(sizeof(*creatures) * n_creatures);
    size_t creatures_idx = 0;
    // go through old creatures, duplicate, mutate, and set into new array
    for(size_t i = 0; i < env->n_creatures; i++) {
        if(env->creatures[i]) {
            struct creature* creature1 = env->creatures[i];
            struct creature* creature2 = creature_duplicate(creature1);
            // 0.1% chance to mutate, only creature 2 mutates
            creature_mutate(creature2, 10);
            creatures[creatures_idx] = creature1;
            creatures[creatures_idx + 1] = creature2;
            creatures_idx += 2;
        }
    }
    env->creatures = creatures;
    env->n_creatures = n_creatures;
}

void environment_distribute(struct environment* env) {
    for(size_t i = 0; i < env->n_creatures; i++) {
        point_t p;
        while(1) {
            point_random(&p, env->width, env->height);
            for(size_t j = 0; j < i; j++) {
                // if we find a collision, continue
                if(point_equals(&p, &env->creatures[j]->location)) continue;
            }
            // no collisions found, break
            break;
        }
        creature_set_location(env->creatures[i], p.x, p.y);
    }
}

void environment_destroy(struct environment* env) {
    for(size_t i = 0; i < env->n_creatures; i++) {
        creature_destroy(env->creatures[i]);
    }
    free(env->creatures);
    free(env);
}

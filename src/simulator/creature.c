
#include "creature.h"
#include "common/common.h"
#include "environment.h"
#include <stdlib.h>
#include <string.h>
// public

void creature_init(
    struct creature* creature,
    creature_species_t species,
    size_t n_genes) {
    genome_init(&creature->genome, n_genes);
    creature->species = species;
    creature->state.alive = 1;
    memset(creature->padding, 0, CREATURE_PADDING_SIZE);
}

__attribute__((always_inline)) static creature_action_t get_action(
    struct creature* creature,
    struct environment* env,
    grid_state_t grid_state) {
    creature_action_t action =
        genome_express(&creature->genome, grid_state, env->args->threshold);
    return action;
}

#if defined(THREADED) && THREADED == 1
__attribute__((always_inline)) static struct creature_workqueue_elm*
wrap_action(
    struct creature* creature,
    struct environment* env,
    size_t grid_idx,
    creature_action_t action) {
    struct creature_workqueue_elm* elm = malloc(sizeof(*elm));
    elm->creature = creature;
    elm->env = env;
    elm->grid_idx = grid_idx;
    elm->action = action;
    return elm;
}
void creature_tick(
    struct creature* creature,
    struct environment* env,
    size_t grid_idx,
    grid_state_t grid_state,
    struct ts_queue* workqueue) {
    creature_action_t action = get_action(creature, env, grid_state);
    ts_queue_enqueue(
        workqueue,
        (void*)wrap_action(creature, env, grid_idx, action));
}
#else
void creature_tick(
    struct creature* creature,
    struct environment* env,
    size_t grid_idx,
    grid_state_t grid_state) {
    creature_action_t action = get_action(creature, env, grid_state);
    creature_apply_action(creature, env, grid_idx, action);
}
#endif

void creature_apply_action(
    __attribute__((unused)) struct creature* creature,
    struct environment* env,
    size_t grid_idx,
    creature_action_t action) {

    while(action != ACTION_NONE) {
        if(action & ACTION_MOVE_LEFT) {
            size_t x = common_get_col(grid_idx, env->width);
            if(x > 0) x--;
            size_t new_grid_idx = common_get_idx(
                x,
                common_get_row(grid_idx, env->width),
                env->width);
            environment_move_creature(env, grid_idx, new_grid_idx);
            action &= ~ACTION_MOVE_LEFT;
            break;
        }
        if(action & ACTION_MOVE_RIGHT) {
            size_t x = common_get_col(grid_idx, env->width);
            if(x < env->width - 1) x++;
            size_t new_grid_idx = common_get_idx(
                x,
                common_get_row(grid_idx, env->width),
                env->width);
            environment_move_creature(env, grid_idx, new_grid_idx);
            action &= ~ACTION_MOVE_RIGHT;
            break;
        }
        if(action & ACTION_MOVE_UP) {
            size_t y = common_get_row(grid_idx, env->width);
            if(y > 0) y--;
            size_t new_grid_idx = common_get_idx(
                common_get_col(grid_idx, env->width),
                y,
                env->width);
            environment_move_creature(env, grid_idx, new_grid_idx);
            action &= ~ACTION_MOVE_UP;
            break;
        }
        if(action & ACTION_MOVE_DOWN) {
            size_t y = common_get_row(grid_idx, env->width);
            if(y < env->height - 1) y++;
            size_t new_grid_idx = common_get_idx(
                common_get_col(grid_idx, env->width),
                y,
                env->width);
            environment_move_creature(env, grid_idx, new_grid_idx);
            action &= ~ACTION_MOVE_DOWN;
            break;
        }
    }
}

void creature_mutate(struct creature* creature, float chance) {
    if(randf() < chance) {
        genome_mutate(&creature->genome);
    }
}

bool creature_is_dead(struct creature* c) { return !creature_is_alive(c); }
bool creature_is_alive(struct creature* c) { return c && c->state.alive == 1; }
void creature_kill(struct creature* c) {
    if(c) c->state.alive = 0;
}


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

void creature_tick(
    struct creature* creature,
    struct environment* env,
    grid_state_t grid_state) {
    if(!creature) return;
    creature_action_t action =
        genome_express(&creature->genome, grid_state, env->args->threshold);
    creature_apply_action(creature, env, action);
}

void creature_apply_action(
    struct creature* creature,
    struct environment* env,
    creature_action_t action) {
    size_t grid_idx = environment_get_grid_idx(env, creature);
    size_t grid_size = env->width * env->height;
    if(grid_idx >= grid_size) return;

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

// express the maximum gene
// void creature_tick(
//     struct creature* creature,
//     struct environment* env,
//     int8_t threshold) {
//     gene_t max_expressed_gene = GENE_INVALID;
//     int8_t max_expressed_value = INT8_MIN;

//     enum surroundings surroundings;
//     environment_get_surroundings(env, creature, &surroundings);

//     for(size_t i = 1; i <= GENOME_SIZE; i++) {
//         int8_t expressed =
//             gene_expression_express(&creature->genome, i, &surroundings);
//         if(expressed > threshold && expressed > max_expressed_value) {
//             max_expressed_value = expressed;
//             max_expressed_gene = creature->genome.genes[i].gene;
//         }
//     }
//     genome_apply_action(env, creature, max_expressed_gene);
// }
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

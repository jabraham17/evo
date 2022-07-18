
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

creature_action_t creature_tick(
    struct creature* creature,
    struct environment* env,
    grid_state_t grid_state) {
    creature_action_t action =
        genome_express(&creature->genome, grid_state, env->args->threshold);
    return action;
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


#include "creature.h"
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
    int8_t threshold) {}

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
void creature_mutate(struct creature* creature, size_t chance) {
    if(rand_max(10000) <= (int)chance) {
        genome_mutate(&creature->genome);
    }
}

bool creature_is_dead(struct creature* c) { return c->state.alive == 0; }
bool creature_is_alive(struct creature* c) { return c->state.alive == 1; }
void creature_kill(struct creature* c) { c->state.alive = 0; }

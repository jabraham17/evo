
#include "creature.h"
#include "environment.h"
#include <stdlib.h>
#include <string.h>
// public

struct creature* creature_create(
    creature_species_t species, size_t n_genes, size_t max_inputs,
    size_t max_outputs) {
    struct creature* creature = calloc(1, sizeof(*creature));
    creature->species = species;
    genome_init(&creature->genome, n_genes, max_inputs, max_outputs);
    return creature;
}

void creature_set_location(struct creature* creature, size_t x, size_t y) {
    creature->location.x = x;
    creature->location.y = y;
}

// express the maximum gene
void creature_tick(
    struct creature* creature, struct environment* env, int8_t threshold) {
    gene_t max_expressed_gene = GENE_INVALID;
    int8_t max_expressed_value = INT8_MIN;

    enum surroundings surroundings;
    environment_get_surroundings(env, creature, &surroundings);

    for(size_t i = 1; i <= GENOME_SIZE; i++) {
        int8_t expressed =
            gene_expression_express(&creature->genome, i, &surroundings);
        if(expressed > threshold && expressed > max_expressed_value) {
            max_expressed_value = expressed;
            max_expressed_gene = creature->genome.genes[i].gene;
        }
    }
    genome_apply_action(env, creature, max_expressed_gene);
}
void creature_mutate(struct creature* creature, size_t chance) {
    if(rand_max(10000) <= (int)chance) {
        genome_mutate(&creature->genome);
    }
}
struct creature* creature_duplicate(struct creature* creature) {
    struct creature* duplicate = malloc(sizeof(*duplicate));
    memcpy(duplicate, creature, sizeof(*duplicate));
    return duplicate;
}


bool creature_intersect(struct creature* c1, struct creature* c2) {
    return point_equals(&c1->location, &c2->location);
}

bool creature_left(struct creature* c1, struct creature* c2) {
    return point_left(&c1->location, &c2->location);
}
bool creature_right(struct creature* c1, struct creature* c2) {
    return point_right(&c1->location, &c2->location);
}
bool creature_up(struct creature* c1, struct creature* c2) {
    return point_up(&c1->location, &c2->location);
}
bool creature_down(struct creature* c1, struct creature* c2) {
    return point_down(&c1->location, &c2->location);
}
bool creature_adjacent(struct creature* c1, struct creature* c2) {
    return creature_left(c1, c2) || creature_right(c1, c2) ||
           creature_up(c1, c2) || creature_down(c1, c2);
}

void creature_destroy(struct creature* creature) { free(creature); }

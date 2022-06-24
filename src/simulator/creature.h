#ifndef SIMULATOR_CREATURE_H_
#define SIMULATOR_CREATURE_H_

#include "common.h"
#include "genome.h"
#include <stdbool.h>
#include <stdint.h>

struct environment;

typedef enum { SPECIES_A } creature_species_t;

struct creature {
    creature_species_t species;
    point_t location;
    genome_t genome;
};

struct creature* creature_create(
    creature_species_t,
    size_t n_genes,
    size_t max_inputs,
    size_t max_outputs);
void creature_set_location(struct creature* creature, size_t x, size_t y);

void creature_tick(struct creature*, struct environment*, int8_t threshold);
// chance out of a ten thousand to mutate
// 100 is a 1% chance
void creature_mutate(struct creature*, size_t chance);
struct creature* creature_duplicate(struct creature*);

bool creature_intersect(struct creature*, struct creature*);
bool creature_left(struct creature*, struct creature*);
bool creature_right(struct creature*, struct creature*);
bool creature_up(struct creature*, struct creature*);
bool creature_down(struct creature*, struct creature*);
bool creature_adjacent(struct creature*, struct creature*);

void creature_destroy(struct creature*);

#endif

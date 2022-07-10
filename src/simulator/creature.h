#ifndef SIMULATOR_CREATURE_H_
#define SIMULATOR_CREATURE_H_

#include "common.h"
#include "genome.h"
#include <stdbool.h>
#include <stdint.h>

struct environment;
typedef enum { SPECIES_A } creature_species_t;

#define CREATURE_SIZE 192
#define CREATURE_PADDING_SIZE                                                  \
    (CREATURE_SIZE - sizeof(genome_t) - sizeof(creature_species_t) - 1)
struct creature {
    genome_t genome;
    creature_species_t species;
    struct {
        uint8_t alive : 1;
    } state;
    uint8_t padding[CREATURE_PADDING_SIZE];
};
_Static_assert(sizeof(struct creature) == CREATURE_SIZE, "");

void creature_init(struct creature*, creature_species_t, size_t n_genes);

void creature_tick(
    struct creature*,
    struct environment*,
    grid_state_t,
    int8_t threshold);
void creature_apply_action(
    struct creature*,
    struct environment*,
    creature_action_t);

// chance out of a ten thousand to mutate
// 100 is a 1% chance
void creature_mutate(struct creature*, size_t chance);
#define MUTATION_RATE_100 10000
#define MUTATION_RATE_75 7500
#define MUTATION_RATE_50 5000
#define MUTATION_RATE_25 2500
#define MUTATION_RATE_10 1000
#define MUTATION_RATE_5 500
#define MUTATION_RATE_1 100
#define MUTATION_RATE_05 50
#define MUTATION_RATE_01 10
#define MUTATION_RATE_0 0

bool creature_is_dead(struct creature*);
bool creature_is_alive(struct creature*);
void creature_kill(struct creature*);

#endif

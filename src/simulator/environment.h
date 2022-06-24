#ifndef SIMULATOR_ENVIRONMENT_H_
#define SIMULATOR_ENVIRONMENT_H_

#include "creature.h"
#include <stddef.h>
#include <stdint.h>

struct environment {
    size_t width;
    size_t height;
    struct creature** creature_grid;
};

enum surroundings {
    S_NONE = 0x0,
    S_WALL_LEFT = 0x1,
    S_WALL_RIGHT = 0x2,
    S_WALL_UP = 0x4,
    S_WALL_DOWN = 0x8,
    S_CREATURE_LEFT = 0x10,
    S_CREATURE_RIGHT = 0x20,
    S_CREATURE_UP = 0x40,
    S_CREATURE_DOWN = 0x80,
};

enum selection_criteria { SELECTION_LEFT };

struct environment* environment_create(size_t width, size_t height);
void environment_populate(struct environment*, size_t n_creatures);

void environment_get_surroundings(
    struct environment*, struct creature*, enum surroundings*);


void environment_run_generation(
    struct environment*, size_t microcount, int8_t threshold);
void environment_next_generation(struct environment*);
void environment_microtick(struct environment*, int8_t threshold);


void environment_select(
    struct environment*, enum selection_criteria selection_criteria);
// mutation invalidates all locations!!!!
void environment_mutate(struct environment*);
void environment_distribute(struct environment*);


void environment_destroy(struct environment*);

#endif



#ifndef SIMULATOR_GENE_H_
#define SIMULATOR_GENE_H_

#include <stdint.h>
#include <stddef.h>

struct environment;
struct creature; 
enum surroundings;

#define GENE_PACKED __attribute__((packed))

// uint8_t
typedef enum {
    GENE_INVALID,

    // gene senses
    GENE_SENSE_WALL_LEFT,
    GENE_SENSE_WALL_RIGHT,
    GENE_SENSE_WALL_UP,
    GENE_SENSE_WALL_DOWN,
    GENE_SENSE_CREATURE_LEFT,
    GENE_SENSE_CREATURE_RIGHT,
    GENE_SENSE_CREATURE_UP,
    GENE_SENSE_CREATURE_DOWN,

    // gene actions
    GENE_ACTION_MOVE_LEFT,
    GENE_ACTION_MOVE_RIGHT,
    GENE_ACTION_MOVE_UP,
    GENE_ACTION_MOVE_DOWN,

    GENE_INTERNAL,

} gene_t;
#define GENE_SIZE (GENE_INTERNAL - GENE_INVALID - 1)
// inclusive
#define GENE_FIRST (GENE_SENSE_WALL_LEFT)
#define GENE_LAST (GENE_INTERNAL)
#define GENE_FIRST_SENSE (GENE_SENSE_WALL_LEFT)
#define GENE_LAST_SENSE (GENE_SENSE_CREATURE_DOWN)
#define GENE_FIRST_ACTION (GENE_ACTION_MOVE_LEFT)
#define GENE_LAST_ACTION (GENE_ACTION_MOVE_DOWN)

// uint8_t
typedef enum {
    ACT_LINEAR,
    ACT_SIGMOID,
    // ACT_RELU,
    // ACT_TANH,
} gene_activation_t;
// inclusive
#define ACTIVATION_FIRST (ACT_SIGMOID)
#define ACTIVATION_LAST (ACT_SIGMOID)

#define GENE_MAX_INPUTS 8
#define GENE_MAX_OUTPUTS 8

typedef struct {
    uint8_t gene;
    int8_t weight; // between -128 and 127, scaled to whatever we decide later
    uint8_t activation;

    uint8_t inputs[GENE_MAX_INPUTS];   // index into genome, an index of zero is
                                       // no input
    uint8_t outputs[GENE_MAX_OUTPUTS]; // index into genome, an index of zero is
                                       // no output
} GENE_PACKED gene_expression_t;

#define GENOME_SIZE 255

typedef struct {
    gene_expression_t genes[GENOME_SIZE+1]; // gene zero must be empty, so one extra
} GENE_PACKED genome_t;

// gene operation
// each input contributes from -1 to 1, times its weight
// each gene takes the inputs, sums them, then applies activation function to
// cap between -1 and 1, then apply weight

// gene_expression_t inputs only apply to internal nodes and sense nodes
// gene_expression_t outputs only apply to internal nodes and action nodes

void gene_expression_init(
    gene_expression_t*, size_t gene_idx, size_t max_inputs,
    size_t max_outputs);

int8_t gene_expression_express(genome_t*, size_t idx, enum surroundings* surroundings);

void genome_init(
    genome_t* genome, size_t n_genes, size_t max_inputs, size_t max_outputs);

void genome_apply_action(struct environment*,  struct creature*, gene_t);
void genome_mutate(genome_t*);

#endif



#ifndef SIMULATOR_GENE_H_
#define SIMULATOR_GENE_H_

#include <stddef.h>
#include <stdint.h>

struct environment;
struct creature;
enum surroundings;

#define GENE_PACKED __attribute__((packed))

typedef enum {
    GENE_INPUT_WALL_LEFT,
    GENE_INPUT_WALL_RIGHT,
    GENE_INPUT_WALL_UP,
    GENE_INPUT_WALL_DOWN,
    GENE_INPUT_CREATURE_LEFT,
    GENE_INPUT_CREATURE_RIGHT,
    GENE_INPUT_CREATURE_UP,
    GENE_INPUT_CREATURE_DOWN,
} gene_input_t;
#define GENE_INPUT_START (GENE_INPUT_WALL_LEFT)
#define GENE_INPUT_END (GENE_INPUT_CREATURE_DOWN)
#define GENE_INPUT_SIZE (GENE_INPUT_END - GENE_INPUT_START + 1)
#define GENE_INPUT_GET(_input) ((_input) % GENE_INPUT_SIZE)

typedef enum {
    GENE_OUTPUT_MOVE_LEFT,
    GENE_OUTPUT_MOVE_RIGHT,
    GENE_OUTPUT_MOVE_UP,
    GENE_OUTPUT_MOVE_DOWN,
} gene_output_t;
#define GENE_OUTPUT_START (GENE_OUTPUT_MOVE_LEFT)
#define GENE_OUTPUT_END (GENE_OUTPUT_MOVE_DOWN)
#define GENE_OUTPUT_SIZE (GENE_OUTPUT_END - GENE_OUTPUT_START + 1)
#define GENE_OUTPUT_GET(_input) ((_input) % GENE_OUTPUT_SIZE)


typedef enum {
    GENE_INTERNAL_N0,
    GENE_INTERNAL_N1,
    GENE_INTERNAL_N2,
} gene_internal_t;
#define GENE_INTERNAL_START (GENE_INTERNAL_N0)
#define GENE_INTERNAL_END (GENE_INTERNAL_N2)
#define GENE_INTERNAL_SIZE (GENE_INTERNAL_END - GENE_INTERNAL_START + 1)
#define GENE_INTERNAL_GET(_input) ((_input) % GENE_INTERNAL_SIZE)

typedef struct {
    union {
        struct {
            uint8_t source_type : 1; // 1 for input, 0 for internal
            uint8_t source_id : 7;
            uint8_t sink_type : 1; // 1 for output, 0 for internal
            uint8_t sink_id : 7;
        };
        uint16_t gene;
    };
    int16_t weight : 16;
} GENE_PACKED connection_t;


#define MAX_CONNECTIONS 256
typedef struct {
    connection_t connections[MAX_CONNECTIONS];
    size_t n_connections;
} GENE_PACKED genome_t;

// gene operation
// each input contributes from -1 to 1, times its weight
// each gene takes the inputs, sums them, then applies activation function to
// cap between -1 and 1, then apply weight

// gene_expression_t inputs only apply to internal nodes and sense nodes
// gene_expression_t outputs only apply to internal nodes and action nodes

int8_t genome_express(genome_t* genome, enum surroundings* surroundings);

void genome_init(genome_t* genome, size_t n_connections);

#endif

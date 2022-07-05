

#ifndef SIMULATOR_GENE_H_
#define SIMULATOR_GENE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint32_t grid_state_t;

typedef struct {
    union {
        struct {
            uint8_t source_type : 1; // 1 for input, 0 for internal
            uint8_t source_id : 7;
            uint8_t sink_type : 1; // 1 for output, 0 for internal
            uint8_t sink_id : 7;
        };
        uint16_t gene;
        struct {
            uint8_t source;
            uint8_t sink;
        };
    };
    int16_t weight : 16;
} __attribute__((packed)) connection_t;

// max size in bytes
#define GENOME_T_SIZE 128
#define MAX_CONNECTIONS                                                        \
    ((GENOME_T_SIZE - sizeof(uint32_t)) / sizeof(connection_t))
struct genome {
    connection_t connections[MAX_CONNECTIONS];
    uint32_t n_connections;
};
typedef struct genome genome_t;
_Static_assert(sizeof(genome_t) == GENOME_T_SIZE, "");

// gene operation
// each input contributes from -1 to 1, times its weight
// each gene takes the inputs, sums them, then applies activation function to
// cap between -1 and 1, then apply weight

// gene_expression_t inputs only apply to internal nodes and sense nodes
// gene_expression_t outputs only apply to internal nodes and action nodes

void genome_init(genome_t* genome, size_t n_connections);

void genome_prune(genome_t* genome);

uint8_t genome_connection_source(connection_t c);
char* genome_connection_source_str(connection_t c);
bool genome_connection_source_is_input(connection_t c);
bool genome_connection_source_is_internal(connection_t c);
uint16_t genome_connection_source_unique_id(connection_t c);

uint8_t genome_connection_sink(connection_t c);
char* genome_connection_sink_str(connection_t c);
bool genome_connection_sink_is_output(connection_t c);
bool genome_connection_sink_is_internal(connection_t c);
uint16_t genome_connection_sink_unique_id(connection_t c);

int16_t genome_connection_weight(connection_t c);

int8_t genome_express(genome_t* genome, grid_state_t grid_state);
void genome_mutate(genome_t* genome);

#endif

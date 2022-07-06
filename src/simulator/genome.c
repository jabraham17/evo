#include "genome.h"
#include "common/macro_map.h"
#include "creature.h"
#include "environment.h"
#include "rand_help.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define GENE_CASE(name) GENE_##name,
#define GENE_CASE_STR(name)                                                    \
    case GENE_##name:                                                          \
        return #name;
#define GENE_FN __attribute__((always_inline, unused)) static
#define GENE_TEMPLATE(classname, first, ...)                                   \
    typedef enum {                                                             \
        GENE_CASE(first) EVAL(MAP(GENE_CASE, __VA_ARGS__))                     \
            LAST_GENE_##classname,                                             \
    } gene_##classname##_t;                                                    \
    GENE_FN gene_##classname##_t gene_##classname##_start() {                  \
        return GENE_##first;                                                   \
    }                                                                          \
    GENE_FN gene_##classname##_t gene_##classname##_end() {                    \
        return (LAST_GENE_##classname - 1);                                    \
    }                                                                          \
    GENE_FN size_t gene_##classname##_size() {                                 \
        return gene_##classname##_end() - gene_##classname##_start() + 1;      \
    }                                                                          \
    GENE_FN gene_##classname##_t gene_##classname##_get(uint8_t input) {       \
        return input % gene_##classname##_size();                              \
    }                                                                          \
    GENE_FN char* gene_##classname##_str(gene_##classname##_t g) {             \
        switch(g) {                                                            \
            GENE_CASE_STR(first)                                               \
            EVAL(MAP(GENE_CASE_STR, __VA_ARGS__))                              \
            default: return "Unknown";                                         \
        }                                                                      \
    }

GENE_TEMPLATE(
    input,
    WALL_LEFT,
    WALL_RIGHT,
    WALL_UP,
    WALL_DOWN,
    CREATURE_LEFT,
    CREATURE_RIGHT,
    CREATURE_UP,
    CREATURE_DOWN)
GENE_TEMPLATE(output, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN)
GENE_TEMPLATE(internal, N0, N1, N2)

#undef GENE_CASE
#undef GENE_CASE_STR
#undef GENE_FN
#undef GENE_TEMPLATE

// typedef enum {
//     GENE_INPUT_WALL_LEFT,
//     GENE_INPUT_WALL_RIGHT,
//     GENE_INPUT_WALL_UP,
//     GENE_INPUT_WALL_DOWN,
//     GENE_INPUT_CREATURE_LEFT,
//     GENE_INPUT_CREATURE_RIGHT,
//     GENE_INPUT_CREATURE_UP,
//     GENE_INPUT_CREATURE_DOWN,
// } gene_input_t;
// #define GENE_INPUT_START (GENE_INPUT_WALL_LEFT)
// #define GENE_INPUT_END (GENE_INPUT_CREATURE_DOWN)
// #define GENE_INPUT_SIZE (GENE_INPUT_END - GENE_INPUT_START + 1)
// #define GENE_INPUT_GET(_input) ((_input) % GENE_INPUT_SIZE)

// typedef enum {
//     GENE_OUTPUT_MOVE_LEFT,
//     GENE_OUTPUT_MOVE_RIGHT,
//     GENE_OUTPUT_MOVE_UP,
//     GENE_OUTPUT_MOVE_DOWN,
// } gene_output_t;
// #define GENE_OUTPUT_START (GENE_OUTPUT_MOVE_LEFT)
// #define GENE_OUTPUT_END (GENE_OUTPUT_MOVE_DOWN)
// #define GENE_OUTPUT_SIZE (GENE_OUTPUT_END - GENE_OUTPUT_START + 1)
// #define GENE_OUTPUT_GET(_input) ((_input) % GENE_OUTPUT_SIZE)

// typedef enum {
//     GENE_INTERNAL_N0,
//     GENE_INTERNAL_N1,
//     GENE_INTERNAL_N2,
// } gene_internal_t;
// #define GENE_INTERNAL_START (GENE_INTERNAL_N0)
// #define GENE_INTERNAL_END (GENE_INTERNAL_N2)
// #define GENE_INTERNAL_SIZE (GENE_INTERNAL_END - GENE_INTERNAL_START + 1)
// #define GENE_INTERNAL_GET(_input) ((_input) % GENE_INTERNAL_SIZE)

#define PRIV_FN __attribute__((unused)) static

// return between -128 and 127
// PRIV_FN int8_t get_sense(enum surroundings* surroundings, gene_t gene) {
//     switch(gene) {
//         case GENE_SENSE_WALL_LEFT:
//             return (*surroundings & S_WALL_LEFT) ? INT8_MAX : INT8_MIN;
//         case GENE_SENSE_WALL_RIGHT:
//             return (*surroundings & S_WALL_RIGHT) ? INT8_MAX : INT8_MIN;
//         case GENE_SENSE_WALL_UP:
//             return (*surroundings & S_WALL_UP) ? INT8_MAX : INT8_MIN;
//         case GENE_SENSE_WALL_DOWN:
//             return (*surroundings & S_WALL_DOWN) ? INT8_MAX : INT8_MIN;
//         case GENE_SENSE_CREATURE_LEFT:
//             return (*surroundings & S_CREATURE_LEFT) ? INT8_MAX : INT8_MIN;
//         case GENE_SENSE_CREATURE_RIGHT:
//             return (*surroundings & S_CREATURE_RIGHT) ? INT8_MAX : INT8_MIN;
//         case GENE_SENSE_CREATURE_UP:
//             return (*surroundings & S_CREATURE_UP) ? INT8_MAX : INT8_MIN;
//         case GENE_SENSE_CREATURE_DOWN:
//             return (*surroundings & S_CREATURE_DOWN) ? INT8_MAX : INT8_MIN;
//         default: return 0;
//     }
// }

// scale int8 to floatMin and floatMax
PRIV_FN float scale_to_float(int8_t value, float floatMin, float floatMax) {
    float oldMin = INT8_MIN;
    float oldMax = INT8_MAX;

    float result =
        (floatMax - floatMin) * ((float)value - oldMin) / (oldMax - oldMin) +
        floatMin;
    return result;
}
// scale float from floatMin and floatMax
PRIV_FN int8_t scale_to_int8(float value, float floatMin, float floatMax) {
    float newMin = INT8_MIN;
    float newMax = INT8_MAX;

    float result =
        (newMax - newMin) * (value - floatMin) / (floatMax - floatMin) + newMin;
    return (int8_t)result;
}
PRIV_FN float clamp(float value, float min, float max) {
    if(value < min) return min;
    if(value > max) return max;
    return value;
}

// modified sigmoid, scales between -1 and 1
PRIV_FN float activation_sigmoid(float value, float weight) {
    return (2.0f / (1.0f + (float)exp(-1.0f * value * weight))) - 1.0f;
}

PRIV_FN bool genome_gene_is_sunk_to(genome_t* genome, uint8_t gene) {
    for(size_t i = 0; i < genome->n_connections; i++) {
        // somebody somewhere sinks to the gene
        if(genome->connections[i].sink == gene) return true;
    }
    return false;
}
PRIV_FN bool genome_gene_is_sourced(genome_t* genome, uint8_t gene) {
    for(size_t i = 0; i < genome->n_connections; i++) {
        // somebody somewhere uses gene as a source
        if(genome->connections[i].source == gene) return true;
    }
    return false;
}
// }PRIV_FN bool genome_gene_is_sunk_to(genome_t* genome, uint8_t gene) {
//     for(size_t i = 0; i < genome->n_connections; i++) {
//         // somebody somewhere sinks to the gene
//         if(genome_connection_sink(genome->connections[i]) == gene) return
//         true;
//     }
//     return false;
// }
// PRIV_FN bool genome_gene_is_sourced(genome_t* genome, uint8_t gene) {
//     for(size_t i = 0; i < genome->n_connections; i++) {
//         // somebody somewhere uses gene as a source
//         if(genome_connection_source(genome->connections[i]) == gene)
//             return true;
//     }
//     return false;
// }

#undef PRIV_FN

// public

void genome_init(genome_t* genome, size_t n_connections) {
    genome->n_connections = n_connections;
    for(size_t i = 0; i < genome->n_connections; i++) {
        // make rand gene and weight
        genome->connections[i].gene =
            rand_in_range_inclusive(0, UINT16_MAX) & 0xFFFF;
        genome->connections[i].weight =
            rand_in_range_inclusive(INT16_MIN, INT16_MAX) & 0xFFFF;
    }
    genome_normalize(genome);
}

// debug
// #include <stdio.h>

void genome_prune(genome_t* genome) {

    /*
    for each connection
    if source is internal and never sunk to, remove
    if sink is internal and never sourced, remove
    */
    bool changed = true;
    while(changed) {
        changed = false;
        for(size_t i = 0; i < genome->n_connections; i++) {
            connection_t c = genome->connections[i];
            // A: if source and sink are same, points to itself, remove
            // B: if source is internal and never sunk to, remove
            // C: if sink is internal and never sourced, remove
            // bool A = genome_connection_sink_is_internal(c) &&
            //     genome_connection_source_is_internal(c) &&
            //     genome_connection_source(c) == genome_connection_sink(c);
            // bool B = genome_connection_source_is_internal(c) &&
            //     !genome_gene_is_sunk_to(genome,
            //     genome_connection_source(c));
            // bool C = genome_connection_sink_is_internal(c) &&
            // !genome_gene_is_sourced(genome, genome_connection_sink(c));
            bool A = genome_connection_sink_is_internal(c) &&
                     genome_connection_source_is_internal(c) &&
                     c.source == c.sink;
            bool B = genome_connection_source_is_internal(c) &&
                     !genome_gene_is_sunk_to(genome, c.source);
            bool C = genome_connection_sink_is_internal(c) &&
                     !genome_gene_is_sourced(genome, c.sink);
            // fprintf(
            //     stderr,
            //     "%15s -> %-15s A=%d B=%d C=%d\n",
            //     genome_connection_source_str(c),
            //     genome_connection_sink_str(c),
            //     A,
            //     B,
            //     C);
            if(A || B || C) {
                changed = true;
                // overwrite this connection with the last connection
                memmove(
                    &genome->connections[i],
                    &genome->connections[genome->n_connections - 1],
                    sizeof(connection_t));
                // reduce the number of connections by 1
                genome->n_connections--;
                // go back one, we just copied one over the top
                i--;
            }
        }
    }
}

uint8_t genome_connection_source(connection_t c) {
    if(genome_connection_source_is_input(c)) return gene_input_get(c.source_id);
    else return gene_internal_get(c.source_id);
}
char* genome_connection_source_str(connection_t c) {
    if(genome_connection_source_is_input(c))
        return gene_input_str(gene_input_get(c.source_id));
    else return gene_internal_str(gene_internal_get(c.source_id));
}
bool genome_connection_source_is_input(connection_t c) {
    return c.source_type == 1;
}
bool genome_connection_source_is_internal(connection_t c) {
    return c.source_type == 0;
}
uint16_t genome_connection_source_unique_id(connection_t c) {
    int16_t id = 0x0000;
    id |= c.source_type << 7;
    id |= (genome_connection_source_is_input(c) & 0x1) << 8;
    if(genome_connection_source_is_input(c)) id |= gene_input_get(c.source_id);
    else id |= gene_internal_get(c.source_id);
    return id;
}

uint8_t genome_connection_sink(connection_t c) {
    if(genome_connection_sink_is_output(c)) return gene_output_get(c.sink_id);
    else return gene_internal_get(c.sink_id);
}
char* genome_connection_sink_str(connection_t c) {
    if(genome_connection_sink_is_output(c))
        return gene_output_str(gene_output_get(c.sink_id));
    else return gene_internal_str(gene_internal_get(c.sink_id));
}
bool genome_connection_sink_is_output(connection_t c) {
    return c.sink_type == 1;
}
bool genome_connection_sink_is_internal(connection_t c) {
    return c.sink_type == 0;
}
uint16_t genome_connection_sink_unique_id(connection_t c) {
    int16_t id = 0x0000;
    id |= c.sink_type << 7;
    id |= (genome_connection_sink_is_output(c) & 0x1) << 9;
    if(genome_connection_sink_is_output(c)) id |= gene_output_get(c.sink_id);
    else id |= gene_internal_get(c.sink_id);
    return id;
}

int16_t genome_connection_weight(connection_t c) { return c.weight; }

// int8_t genome_express(genome_t* genome, enum surroundings* surroundings)
// {
//     if(idx == 0) return INT8_MIN;
//     gene_expression_t* gene_expression = &genome->genes[idx];
//     if(gene_expression->gene == GENE_INVALID) return INT8_MIN;
//
//     // if sense, return how much its expressed
//     if(is_sense(gene_expression)) {
//         int8_t sense = get_sense(surroundings, gene_expression->gene);
//         float sense_f = scale_to_float(sense, -1.0, 1.0);
//         float weight = scale_to_float(gene_expression->weight,
//         -4.0, 4.0); float activation_f =
//             activation_apply(gene_expression->activation, sense_f,
//             weight);
//         int8_t activation = scale_to_int8(activation_f, -1.0, 1.0);
//         return activation;
//     }
//     // otherwise, we are either internal or action
//     // either way need to compute all inputs,
//     // apply activation, and return expression
//     else {
//         float inputs = 0;
//         for(size_t i = 0; i < GENE_MAX_INPUTS; i++) {
//             // if input is not this gene, express it
//             size_t gene_to_express_idx = gene_expression->inputs[i];
//             if(gene_to_express_idx != idx) {
//                 int8_t expressed = gene_expression_express(
//                     genome,
//                     gene_to_express_idx,
//                     surroundings);
//                 inputs += scale_to_float(expressed, -1.0, 1.0);
//             }
//         }
//         // activate and return
//         float weight = scale_to_float(gene_expression->weight,
//         -4.0, 4.0); float activation_f =
//             activation_apply(gene_expression->activation, inputs,
//             weight);
//         int8_t activation = scale_to_int8(activation_f, -1.0, 1.0);
//         return activation;
//     }
// }

void genome_normalize(genome_t* genome) {
    for(size_t i = 0; i < genome->n_connections; i++) {
        genome->connections[i].source_id =
            genome_connection_source(genome->connections[i]);
        genome->connections[i].sink_id =
            genome_connection_sink(genome->connections[i]);
    }
}

int8_t genome_express(genome_t* genome, grid_state_t grid_state) {}
void genome_mutate(genome_t* genome) {
    // randomly select a gene and mutate it
    size_t gene_idx = rand_max(genome->n_connections);
    genome->connections[gene_idx].gene =
        rand_in_range_inclusive(0, UINT16_MAX) & 0xFFFF;
    genome->connections[gene_idx].weight =
        rand_in_range_inclusive(INT16_MIN, INT16_MAX) & 0xFFFF;

    // after a mutation, normalize and prune
    genome_normalize(genome);
    genome_prune(genome);
}

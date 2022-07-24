#include "genome.h"
#include "common/common.h"
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
    CREATURE_DOWN,
    CREATURE_MASS)
GENE_TEMPLATE(output, MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN)
GENE_TEMPLATE(internal, N0, N1, N2)

#undef GENE_CASE
#undef GENE_CASE_STR
#undef GENE_FN
#undef GENE_TEMPLATE

#define PRIV_FN __attribute__((unused)) static

// modified sigmoid, scales between -1 and 1
PRIV_FN __attribute__((always_inline)) float
activation_sigmoid(float value, float weight) {
    return (2.0f / (1.0f + (float)exp(-1.0f * value * weight))) - 1.0f;
}

#ifdef __x86_64__
    #include <immintrin.h>
PRIV_FN __attribute__((always_inline)) float
activation_sigmoid_approx(float value, float weight) {
    return clampf(
        value * weight *
            _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(1.0f + value * value))),
        -1.0f,
        1.0f);
}
// PRIV_FN __attribute__((always_inline)) int32_t
// activation_sigmoid_approx3(int32_t value, int32_t weight) {
//     __m128 value_f = _mm_cvt_si2ss(_mm_set_ps1(0), value);
//     __m128 weight_f = _mm_cvt_si2ss(_mm_set_ps1(0), weight);
//     __m128 value_weight = _mm_mul_ss(value_f, weight_f);
//     __m128 value_2 = _mm_mul_ss(value_f, value_f);
//     __m128 value_2_plus_1 = _mm_add_ss(value_2, _mm_set_ss(1.0f));
//     __m128 result = _mm_mul_ss(value_weight, _mm_rsqrt_ss(value_2_plus_1));
//     return _mm_cvt_ss2si(result);
// }
#else
PRIV_FN __attribute__((always_inline)) float
activation_sigmoid_approx(float value, float weight) {
    return clampf(
        (value * weight) * (1 / sqrt(1 + value * value)),
        -1.0f,
        1.0f);
}
#endif

// PRIV_FN int8_t gene_sense(uint8_t gene, grid_state_t state) {
// #define SENSE_CASE(name) \
//     case GENE_##name: return (state & S_##name) ? INT8_MAX : INT8_MIN
//     switch(gene) {
//         SENSE_CASE(WALL_LEFT);
//         SENSE_CASE(WALL_RIGHT);
//         SENSE_CASE(WALL_UP);
//         SENSE_CASE(WALL_DOWN);
//         SENSE_CASE(CREATURE_LEFT);
//         SENSE_CASE(CREATURE_RIGHT);
//         SENSE_CASE(CREATURE_UP);
//         SENSE_CASE(CREATURE_DOWN);
//         default: return 0;
//     }
// #undef SENSE_CASE
// }

#ifdef __x86_64__
    #include <immintrin.h>
    #define popcnt _mm_popcnt_u32
#else
    #define popcnt __builtin_popcount
#endif
PRIV_FN float gene_sensef(uint8_t gene, grid_state_t state) {
#define SENSE_CASE(name)                                                       \
    case GENE_##name: return (state & S_##name) ? -1.0 : 1.0
    switch(gene) {
        SENSE_CASE(WALL_LEFT);
        SENSE_CASE(WALL_RIGHT);
        SENSE_CASE(WALL_UP);
        SENSE_CASE(WALL_DOWN);
        SENSE_CASE(CREATURE_LEFT);
        SENSE_CASE(CREATURE_RIGHT);
        SENSE_CASE(CREATURE_UP);
        SENSE_CASE(CREATURE_DOWN);
        case GENE_CREATURE_MASS:
            return scale_b2f(
                popcnt(
                    state & (S_CREATURE_LEFT & S_CREATURE_RIGHT &
                             S_CREATURE_UP & S_CREATURE_DOWN)),
                0,
                4,
                -1.0,
                1.0);
        default: return 0;
    }
#undef SENSE_CASE
}
PRIV_FN int8_t gene_action(uint8_t gene) {
    switch(gene) {
        case GENE_MOVE_LEFT: return ACTION_MOVE_LEFT;
        case GENE_MOVE_RIGHT: return ACTION_MOVE_RIGHT;
        case GENE_MOVE_UP: return ACTION_MOVE_UP;
        case GENE_MOVE_DOWN: return ACTION_MOVE_DOWN;
        default: return 0;
    }
}

PRIV_FN float gene_expressf(genome_t* genome, size_t idx, grid_state_t state) {
    // express 1 gene, follow connection chain
    // given current idx to a connection, find everyone who sinks to the source
    // recursively call, sum and apply activation

    float inputs = 0;
    connection_t gene_connection = genome->connections[idx];
    uint8_t source = genome_connection_source(gene_connection);
    int16_t weight = genome_connection_weight(gene_connection);

    if(genome_connection_source_is_input(gene_connection)) {
        float expressed = gene_sensef(source, state);
        inputs = expressed;
    } else if(!genome_connection_sink_is_output(gene_connection)) {
        for(size_t i = 0; i < genome->n_connections; i++) {
            if(i == idx) continue;
            // if a connection sinks to the source, we need to calculate it
            if(genome_connection_sink(genome->connections[i]) == source) {
                float expressed = gene_expressf(genome, i, state);
                inputs += expressed;
            }
        }
    }

    float weight_f = scale_w2f(weight, INT16_MIN, INT16_MAX, -4.0, 4.0);
    float activated = activation_sigmoid_approx(inputs, weight_f);
    return activated;
}

PRIV_FN __attribute__((always_inline)) int8_t
gene_express(genome_t* genome, size_t idx, grid_state_t state) {
    float expressed = gene_expressf(genome, idx, state);
    int8_t scaled = scale_f2b(expressed * 128, -1.0, 1.0, INT8_MIN, INT8_MAX);
    return scaled;
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
    genome_prune(genome);
}

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
            bool A = genome_connection_sink_is_internal(c) &&
                     genome_connection_source_is_internal(c) &&
                     c.source == c.sink;
            bool B = genome_connection_source_is_internal(c) &&
                     !genome_gene_is_sunk_to(genome, c.source);
            bool C = genome_connection_sink_is_internal(c) &&
                     !genome_gene_is_sourced(genome, c.sink);
            if(A || B || C) {
                changed = true;
                // overwrite this connection with the last connection
                memmove(
                    &genome->connections[i],
                    &genome->connections[genome->n_connections - 1],
                    sizeof(connection_t));
                memset(
                    &genome->connections[genome->n_connections - 1],
                    0,
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

creature_action_t
genome_express(genome_t* genome, grid_state_t grid_state, int8_t threshold) {
    creature_action_t actions = ACTION_NONE;
    // express output genes
    for(size_t i = 0; i < genome->n_connections; i++) {
        connection_t c = genome->connections[i];
        if(genome_connection_sink_is_output(c)) {
            int8_t expressed = gene_express(genome, i, grid_state);
            uint8_t action = gene_action(genome_connection_sink(c));

            if(expressed >= 0 && expressed > threshold) {
                actions |= action;
            } else if(expressed < 0 && abs(expressed) > threshold) {
                actions &= ~action;
            }
        }
    }
    return actions;
}

void genome_normalize(genome_t* genome) {
    for(size_t i = 0; i < genome->n_connections; i++) {
        genome->connections[i].source_id =
            genome_connection_source(genome->connections[i]);
        genome->connections[i].sink_id =
            genome_connection_sink(genome->connections[i]);
    }
}

void genome_mutate(genome_t* genome) {
    // n+1 allows mutations to grow new connections
    int max_index = clampq(genome->n_connections + 1, 0, MAX_CONNECTIONS);
    // randomly select a gene and mutate it
    size_t gene_idx = rand_max(max_index);
    genome->connections[gene_idx].gene =
        rand_in_range_inclusive(0, UINT16_MAX) & 0xFFFF;
    genome->connections[gene_idx].weight =
        rand_in_range_inclusive(INT16_MIN, INT16_MAX) & 0xFFFF;

    // after a mutation, normalize and prune
    genome_normalize(genome);
    genome_prune(genome);
}

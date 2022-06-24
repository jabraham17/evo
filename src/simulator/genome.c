#include "genome.h"
#include "environment.h"
#include "rand_help.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// private

// return between -128 and 127
static int8_t get_sense(enum surroundings* surroundings, gene_t gene) {
    switch(gene) {
        case GENE_SENSE_WALL_LEFT:
            return (*surroundings & S_WALL_LEFT) ? INT8_MAX : INT8_MIN;
        case GENE_SENSE_WALL_RIGHT:
            return (*surroundings & S_WALL_RIGHT) ? INT8_MAX : INT8_MIN;
        case GENE_SENSE_WALL_UP:
            return (*surroundings & S_WALL_UP) ? INT8_MAX : INT8_MIN;
        case GENE_SENSE_WALL_DOWN:
            return (*surroundings & S_WALL_DOWN) ? INT8_MAX : INT8_MIN;
        case GENE_SENSE_CREATURE_LEFT:
            return (*surroundings & S_CREATURE_LEFT) ? INT8_MAX : INT8_MIN;
        case GENE_SENSE_CREATURE_RIGHT:
            return (*surroundings & S_CREATURE_RIGHT) ? INT8_MAX : INT8_MIN;
        case GENE_SENSE_CREATURE_UP:
            return (*surroundings & S_CREATURE_UP) ? INT8_MAX : INT8_MIN;
        case GENE_SENSE_CREATURE_DOWN:
            return (*surroundings & S_CREATURE_DOWN) ? INT8_MAX : INT8_MIN;
        default: return 0;
    }
}

// scale int8 to floatMin and floatMax
static float scale_to_float(int8_t value, float floatMin, float floatMax) {
    float oldMin = INT8_MIN;
    float oldMax = INT8_MAX;

    float result =
        (floatMax - floatMin) * ((float)value - oldMin) / (oldMax - oldMin) +
        floatMin;
    return result;
}
// scale float from floatMin and floatMax
static int8_t scale_to_int8(float value, float floatMin, float floatMax) {
    float newMin = INT8_MIN;
    float newMax = INT8_MAX;

    float result =
        (newMax - newMin) * (value - floatMin) / (floatMax - floatMin) + newMin;
    return (int8_t)result;
}
static float clamp(float value, float min, float max) {
    if(value < min) return min;
    if(value > max) return max;
    return value;
}

// modified sigmoid, scales between -1 and 1
static float activation_sigmoid(float value, float weight) {
    return (2.0f / (1.0f + (float)exp(-1.0f * value * weight))) - 1.0f;
}
// public

int8_t genome_express(genome_t* genome, enum surroundings* surroundings) {
    if(idx == 0) return INT8_MIN;
    gene_expression_t* gene_expression = &genome->genes[idx];
    if(gene_expression->gene == GENE_INVALID) return INT8_MIN;

    // if sense, return how much its expressed
    if(is_sense(gene_expression)) {
        int8_t sense = get_sense(surroundings, gene_expression->gene);
        float sense_f = scale_to_float(sense, -1.0, 1.0);
        float weight = scale_to_float(gene_expression->weight, -4.0, 4.0);
        float activation_f =
            activation_apply(gene_expression->activation, sense_f, weight);
        int8_t activation = scale_to_int8(activation_f, -1.0, 1.0);
        return activation;
    }
    // otherwise, we are either internal or action
    // either way need to compute all inputs,
    // apply activation, and return expression
    else {
        float inputs = 0;
        for(size_t i = 0; i < GENE_MAX_INPUTS; i++) {
            // if input is not this gene, express it
            size_t gene_to_express_idx = gene_expression->inputs[i];
            if(gene_to_express_idx != idx) {
                int8_t expressed = gene_expression_express(
                    genome,
                    gene_to_express_idx,
                    surroundings);
                inputs += scale_to_float(expressed, -1.0, 1.0);
            }
        }
        // activate and return
        float weight = scale_to_float(gene_expression->weight, -4.0, 4.0);
        float activation_f =
            activation_apply(gene_expression->activation, inputs, weight);
        int8_t activation = scale_to_int8(activation_f, -1.0, 1.0);
        return activation;
    }
}

void genome_init(genome_t* genome, size_t n_connections) {
    genome->n_connections = n_connections;
    for(size_t i = 0; i < genome->n_connections; i++) {
        // make rand gene and weight
        genome->connections[i].gene =
            rand_in_range_inclusive(0, UINT16_MAX) & 0xFFFF;
        genome->connections[i].weight =
            rand_in_range_inclusive(INT16_MIN, INT16_MAX) & 0xFFFF;
    }
}

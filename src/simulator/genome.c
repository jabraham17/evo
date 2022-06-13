#include "genome.h"
#include "environment.h"
#include "rand_help.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// private

static void gene_expression_mutate_gene(genome_t* genome, size_t gene_idx) {
    genome->genes[gene_idx].gene = rand_in_range_inclusive(GENE_FIRST, GENE_LAST);
}
static void gene_expression_mutate_weight(genome_t* genome, size_t gene_idx) {
    genome->genes[gene_idx].weight = rand_in_range_inclusive(INT8_MIN, INT8_MAX);
}
static void gene_expression_mutate_activation(genome_t* genome, size_t gene_idx) {
    genome->genes[gene_idx].activation = rand_in_range_inclusive(ACTIVATION_FIRST, ACTIVATION_LAST);
}
static void gene_expression_mutate_inputs(genome_t* genome, size_t gene_idx) {
    int i = rand_in_range_inclusive(0, GENE_MAX_INPUTS - 1);
    int r;
    while((r = rand_in_range_inclusive(0, GENOME_SIZE)) == (int)gene_idx) {
    }
    genome->genes[gene_idx].inputs[i] = r;
}
static void gene_expression_mutate_outputs(genome_t* genome, size_t gene_idx) {
    int i = rand_in_range_inclusive(0, GENE_MAX_OUTPUTS - 1);
    int r;
    while((r = rand_in_range_inclusive(0, GENOME_SIZE)) == (int)gene_idx) {
    }
    genome->genes[gene_idx].outputs[i] = r;
}

static void gene_expression_mutate(genome_t* genome, size_t gene_idx) {
    int what_gets_mutated = rand_max(5);
    switch(what_gets_mutated) {
        case 0: gene_expression_mutate_gene(genome, gene_idx); break;
        case 1: gene_expression_mutate_weight(genome, gene_idx); break;
        case 2: gene_expression_mutate_activation(genome, gene_idx); break;
        case 3: gene_expression_mutate_inputs(genome, gene_idx); break;
        case 4: gene_expression_mutate_outputs(genome, gene_idx); break;
        default: break;
    }
}

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

__attribute__((unused)) static bool
is_sense(gene_expression_t* gene_expression) {
    return gene_expression->gene >= GENE_FIRST_SENSE ||
           gene_expression->gene <= GENE_LAST_SENSE;
}
__attribute__((unused)) static bool
is_action(gene_expression_t* gene_expression) {
    return gene_expression->gene >= GENE_FIRST_ACTION ||
           gene_expression->gene <= GENE_LAST_ACTION;
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

// clamped to -1 and 1
static float activation_linear(float value, float weight) {
    return clamp(value * weight, -1, 1);
}

// modified sigmoid, scales between -1 and 1
static float activation_sigmoid(float value, float weight) {
    return (2.0f / (1.0f + (float)exp(-1.0f * value * weight))) - 1.0f;
}

static float
activation_apply(gene_activation_t act, float value, float weight) {
    switch(act) {
        case ACT_LINEAR: return activation_linear(value, weight);
        case ACT_SIGMOID: return activation_sigmoid(value, weight);
        default: return activation_linear(value, weight);
    }
}

// public

void gene_expression_init(
    gene_expression_t* gene_expression, size_t gene_idx, size_t max_inputs,
    size_t max_outputs) {
    gene_expression->gene = rand_in_range_inclusive(GENE_FIRST, GENE_LAST);
    gene_expression->weight = rand_in_range_inclusive(INT8_MIN, INT8_MAX);
    gene_expression->activation =
        rand_in_range_inclusive(ACTIVATION_FIRST, ACTIVATION_LAST);

    // cannot point to itself
    for(size_t i = 0; i < max_inputs; i++) {
        int r;
        while((r = rand_in_range_inclusive(0, GENOME_SIZE)) == (int)gene_idx) {
        }
        gene_expression->inputs[i] = r;
    }
    for(size_t i = 0; i < max_outputs; i++) {
        int r;
        while((r = rand_in_range_inclusive(0, GENOME_SIZE)) == (int)gene_idx) {
        }
        gene_expression->outputs[i] = r;
    }
}

int8_t gene_expression_express(
    genome_t* genome, size_t idx, enum surroundings* surroundings) {
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
                    genome, gene_to_express_idx, surroundings);
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

void genome_init(
    genome_t* genome, size_t n_genes, size_t max_inputs, size_t max_outputs) {
    // skip gene 0
    for(size_t i = 1; i <= n_genes; i++) {
        gene_expression_init(&genome->genes[i], i, max_inputs, max_outputs);
    }
}

void genome_apply_action(
    struct environment* env, struct creature* creature, gene_t gene) {
    point_t p = creature->location;
    // because as soon as we move the creature it will change its surroundings
    // of everyone else, we have to recompute here
    switch(gene) {
        case GENE_ACTION_MOVE_LEFT: p.x -= (p.x > 0) ? 1 : 0; break;
        case GENE_ACTION_MOVE_RIGHT:
            p.x += (p.x < env->width - 1) ? 1 : 0;
            break;
        case GENE_ACTION_MOVE_UP: p.y -= (p.y > 0) ? 1 : 0; break;
        case GENE_ACTION_MOVE_DOWN:
            p.y += (p.y < env->height - 1) ? 1 : 0;
            break;
        default: break;
    }
    creature_set_location(creature, p.x, p.y);
}

void genome_mutate(genome_t* genome) {
    int n_mutations = rand_in_range_inclusive(1, 3);

    for(int i = 0; i < n_mutations; i++) {
        size_t idx_gene_to_mutate = rand_in_range_inclusive(1, GENOME_SIZE);
        gene_expression_mutate(genome, idx_gene_to_mutate);
    }
}

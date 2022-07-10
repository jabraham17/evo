

#include "viz/viz.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define UNUSED __attribute((unused))

int main(UNUSED int argc, UNUSED char** argv) {
    long seed = time(0);
    fprintf(stderr, "Seed: %ld\n", seed);
    srand(seed);

    struct creature c;
    creature_init(&c, SPECIES_A, 4);
    dgraph_t* dg_preprune = viz_dump_creature(&c);
    genome_prune(&c.genome);
    dgraph_t* dg_pruned = viz_dump_creature(&c);
    // genome_mutate(&c.genome);
    creature_mutate(&c, MUTATION_RATE_100);
    dgraph_t* dg_mutate = viz_dump_creature(&c);

    dgraph_t* dg = dot_create("creature");
    dot_set_directed(dg, true);
    dot_set_attribute(dg, "splines", "curved");
    dot_add_subgraph(dg, dg_preprune);
    dot_add_subgraph(dg, dg_pruned);
    dot_add_subgraph(dg, dg_mutate);

    char buf[8192];
    if(dot_to_string(dg, buf, 8192)) {
        size_t buflen = strlen(buf);
        if(argc == 2) {
            char* filename = argv[1];
            FILE* fp = fopen(filename, "w");
            fwrite(buf, buflen, 1, fp);
            fclose(fp);
        } else {
            fwrite(buf, buflen, 1, stdout);
        }
    } else {
        fprintf(stderr, "Failed to write dot\n");
    }
    dot_destroy(dg);

    return 0;
}



#include "viz/viz.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define UNUSED __attribute((unused))

int main(UNUSED int argc, UNUSED char** argv) {
    srand(time(0));

    struct creature c;
    creature_init(&c, SPECIES_A, 10);
    dgraph_t* dg_preprune = viz_dump_creature(&c);
    fprintf(stderr, "n connections %d\n", c.genome.n_connections);
    genome_prune(&c.genome);
    fprintf(stderr, "n connections %d\n", c.genome.n_connections);
    dgraph_t* dg_pruned = viz_dump_creature(&c);

    dgraph_t* dg = dot_create("creature");
    dot_set_directed(dg, true);
    dot_set_attribute(dg, "splines", "curved");
    dot_add_subgraph(dg, dg_preprune);
    dot_add_subgraph(dg, dg_pruned);

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
    // dot_destroy(dg_preprune);
    // dot_destroy(dg_pruned);

    return 0;
}

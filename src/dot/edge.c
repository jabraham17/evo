#include "common/utlist.h"
#include "dot.h"
#include "private_defs.inc"
#include <stdlib.h>

struct edge {
    unsigned long first;
    unsigned long second;
    struct attribute* attrs;
};

static void edge_destroy_one(struct edge* edge) {
    attribute_destroy_all(edge->attrs) free(edge);
}
static void edge_destroy_all(struct edge* edges) {
    struct edge *edge_elm, *edge_tmp;
    LL_FOREACH_SAFE(edges, edge_elm, edge_tmp) {
        LL_DELETE(edges, edge_elm);
        edge_destroy_one(edge_elm);
    }
}

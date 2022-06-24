#include "common/utlist.h"
#include "dot.h"
#include "private_defs.inc"
#include <stdlib.h>
#include <string.h>

struct edge* edge_create(unsigned long first, unsigned long second) {
    struct edge* edge = malloc(sizeof(*edge));
    edge_init(edge, first, second);
    return edge;
}
void edge_init(struct edge* edge, unsigned long first, unsigned long second) {
    memset(edge, 0, sizeof(*edge));
    edge->first = first;
    edge->second = second;
}

void edge_destroy_one(struct edge* edge) {
    attribute_destroy_all(edge->attrs);
    free(edge);
}
void edge_destroy_all(struct edge* edges) {
    struct edge *edge_elm, *edge_tmp;
    LL_FOREACH_SAFE(edges, edge_elm, edge_tmp) {
        LL_DELETE(edges, edge_elm);
        edge_destroy_one(edge_elm);
    }
}

unsigned long edge_first_id(struct edge* edge) { return edge->first; }
unsigned long edge_second_id(struct edge* edge) { return edge->second; }

struct attribute** edge_attributes(struct edge* edge) {
    return &edge->attrs;
}

struct edge*
edge_search(struct edge* edges, unsigned long first, unsigned long second) {
    struct edge edge;
    edge_init(&edge, first, second);

    struct edge* elm;
    LL_SEARCH(edges, elm, &edge, edge_cmp);

    return elm;
}

// 0 if equal, hence the !
int edge_cmp(struct edge* a, struct edge* b) {
    return !(a->first == b->first && a->second == b->second);
}

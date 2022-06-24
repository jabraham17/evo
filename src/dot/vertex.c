#include "common/utlist.h"
#include "dot.h"
#include "private_defs.inc"
#include <stdlib.h>
#include <string.h>

struct vertex* vertex_create(unsigned long id) {
    struct vertex* vertex = malloc(sizeof(*vertex));
    vertex_init(vertex, id);
    return vertex;
}
void vertex_init(struct vertex* vertex, unsigned long id) {
    memset(vertex, 0, sizeof(*vertex));
    vertex->id = id;
}

void vertex_destroy_one(struct vertex* vertex) {
    attribute_destroy_all(vertex->attrs);
    free(vertex);
}
void vertex_destroy_all(struct vertex* vertexes) {
    struct vertex *vertex_elm, *vertex_tmp;
    LL_FOREACH_SAFE(vertexes, vertex_elm, vertex_tmp) {
        LL_DELETE(vertexes, vertex_elm);
        vertex_destroy_one(vertex_elm);
    }
}

unsigned long vertex_id(struct vertex* vertex) { return vertex->id; }

struct attribute** vertex_attributes(struct vertex* vertex) {
    return &vertex->attrs;
}

struct vertex* vertex_search(struct vertex* vertexes, unsigned long id) {
    struct vertex vertex;
    vertex_init(&vertex, id);

    struct vertex* elm;
    LL_SEARCH(vertexes, elm, &vertex, vertex_cmp);

    return elm;
}

// 0 if equal, hence the !
int vertex_cmp(struct vertex* a, struct vertex* b) { return !(a->id == b->id); }

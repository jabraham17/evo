#include "dot.h"
#include "private_defs.inc"
#include "common/utlist.h"
#include <stdlib.h>

struct vertex {
    unsigned long id;
    struct attribute* attrs;
    struct vertex* next;
};

static void vertex_destroy_one(struct vertex* vertex) {
    attribute_destroy_all(vertex->attrs)
    free(vertex);
}
static void vertex_destroy_all(struct vertex* vertexes) {
    struct vertex *vertex_elm, *vertex_tmp;
    LL_FOREACH_SAFE(vertexes, vertex_elm, vertex_tmp) {
        LL_DELETE(vertexes, vertex_elm);
        vertex_destroy_one(vertex_elm);
    }
}

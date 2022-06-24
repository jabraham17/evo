#include "common/utlist.h"
#include "dot.h"
#include "private_defs.inc"
#include <stdlib.h>
#include <string.h>

struct dot_graph {
    char name[MAX_ATTRIBUTE_LENGTH];
    bool directed;
    struct vertex* vertexes;
    struct edge* edges;
    struct attribute* attrs;
    struct dot_graph* parent;
    struct dot_graph* children;
};

struct dot_graph* dot_create(char* name) {
    struct dot_graph* graph = malloc(sizeof(*graph));
    dot_init(graph, name);
    return graph;
}
void dot_init(struct dot_graph* graph, char* name) {
    memset(graph, 0, sizeof(*graph));
    if(name) strncpy(graph->name, name, MAX_ATTRIBUTE_LENGTH - 1);
}
// frees everything except the parent
void dot_destroy(struct dot_graph* graph) {

    vertex_destroy_all(graph->vertexes);
    edge_destroy_all(graph->edges);
    attribute_destroy_all(graph->attrs);

    struct dot_graph *graph_elm, *graph_tmp;
    LL_FOREACH_SAFE(graph->children, graph_elm, graph_tmp) {
        LL_DELETE(graph->children, graph_elm);
        dot_destroy(graph_elm);
    }

    // free the structure
    free(graph);
}

bool dot_to_string(struct dot_graph* graph, char* buffer, size_t buffer_size) {}

void dot_set_directed(struct dot_graph* graph, bool directed) {
    graph->directed = directed;
}
bool dot_is_directed(struct dot_graph* graph) { return graph->directed; }

bool dot_is_subgraph(struct dot_graph* graph) { return graph->parent == NULL; }
void dot_add_subgraph(struct dot_graph* graph, struct dot_graph* subgraph) {
    subgraph->parent = graph;
    LL_APPEND(graph->children, subgraph);
}

void dot_set_attribute(struct dot_graph* graph, char* key, char* value) {
    attribute_set(&graph->attrs, key, value);
}
char* dot_get_attribute(struct dot_graph* graph, char* key) {
    return attribute_get(&graph->attrs, key);
}
bool dot_has_attribute(struct dot_graph* graph, char* key) {
    return attribute_get(&graph->attrs, key) != NULL;
}
bool dot_remove_attribute(struct dot_graph* graph, char* key) {
    return (bool)attribute_remove(&graph->attrs, key);
}

bool dot_add_vertex(struct dot_graph* graph, unsigned long id) {
    struct vertex* vertex = vertex_create(id);
    LL_APPEND(graph->vertexes, vertex);
}
bool dot_set_attribute_on_vertex(
    struct dot_graph* graph,
    unsigned long id,
    char* key,
    char* value) {
    struct vertex* vertex = vertex_search(graph->vertexes, id);
}
char* dot_get_attribute_on_vertex(
    struct dot_graph* graph,
    unsigned long id,
    char* key) {}
bool dot_has_attribute_on_vertex(
    struct dot_graph* graph,
    unsigned long id,
    char* key) {}
bool dot_remove_attribute_on_vertex(
    struct dot_graph* graph,
    unsigned long id,
    char* key) {}

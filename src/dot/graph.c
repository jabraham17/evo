#include "common/utlist.h"
#include "dot.h"
#include "private_defs.inc"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dot_graph {
    char name[MAX_ATTRIBUTE_LENGTH];
    bool directed;
    struct vertex* vertexes;
    struct edge* edges;
    struct attribute* attrs;
    dgraph_t* parent;
    dgraph_t* children;
};

dgraph_t* dot_create(char* name) {
    dgraph_t* graph = malloc(sizeof(*graph));
    dot_init(graph, name);
    return graph;
}
void dot_init(dgraph_t* graph, char* name) {
    memset(graph, 0, sizeof(*graph));
    if(name) strncpy(graph->name, name, MAX_ATTRIBUTE_LENGTH - 1);
}
// frees everything except the parent
void dot_destroy(dgraph_t* graph) {

    vertex_destroy_all(graph->vertexes);
    edge_destroy_all(graph->edges);
    attribute_destroy_all(graph->attrs);

    dgraph_t *graph_elm, *graph_tmp;
    LL_FOREACH_SAFE2(graph->children, graph_elm, graph_tmp, children) {
        LL_DELETE2(graph->children, graph_elm, children);
        dot_destroy(graph_elm);
    }

    // free the structure
    free(graph);
}

bool dot_to_string(dgraph_t* graph, char* buffer, size_t buffer_size) {
    memset(buffer, 0, buffer_size);
    return graph_to_string(buffer, buffer_size, graph, 0) >= 0;
}

void dot_set_directed(dgraph_t* graph, bool directed) {
    graph->directed = directed;
}
bool dot_is_directed(dgraph_t* graph) { return graph->directed; }

bool dot_is_subgraph(dgraph_t* graph) { return graph->parent != NULL; }
void dot_add_subgraph(dgraph_t* graph, dgraph_t* subgraph) {
    subgraph->parent = graph;
    LL_APPEND2(graph->children, subgraph, children);
}

void dot_set_attribute(dgraph_t* graph, char* key, char* value) {
    attribute_set(&graph->attrs, key, value);
}
char* dot_get_attribute(dgraph_t* graph, char* key) {
    return attribute_get(graph->attrs, key);
}
bool dot_has_attribute(dgraph_t* graph, char* key) {
    return attribute_get(graph->attrs, key) != NULL;
}
bool dot_remove_attribute(dgraph_t* graph, char* key) {
    return (bool)attribute_remove(&graph->attrs, key);
}
bool dot_add_vertex(dgraph_t* graph, unsigned long id) {
    struct vertex* vertex = vertex_search(graph->vertexes, id);
    if(!vertex) {
        vertex = vertex_create(id);
        LL_APPEND(graph->vertexes, vertex);
        return true;
    }
    return false;
}
bool dot_set_attribute_on_vertex(
    dgraph_t* graph,
    unsigned long id,
    char* key,
    char* value) {
    struct vertex* vertex = vertex_search(graph->vertexes, id);
    if(vertex) {
        struct attribute** attrs = vertex_attributes(vertex);
        attribute_set(attrs, key, value);
        return true;
    } else return false;
}
char* dot_get_attribute_on_vertex(
    dgraph_t* graph,
    unsigned long id,
    char* key) {
    struct vertex* vertex = vertex_search(graph->vertexes, id);
    if(vertex) {
        struct attribute** attrs = vertex_attributes(vertex);
        return attribute_get(*attrs, key);
    } else return NULL;
}
bool dot_has_attribute_on_vertex(dgraph_t* graph, unsigned long id, char* key) {
    return dot_get_attribute_on_vertex(graph, id, key) != NULL;
}
bool dot_remove_attribute_on_vertex(
    dgraph_t* graph,
    unsigned long id,
    char* key) {
    struct vertex* vertex = vertex_search(graph->vertexes, id);
    if(vertex) {
        struct attribute** attrs = vertex_attributes(vertex);
        return attribute_remove(attrs, key);
    } else return false;
}

bool dot_add_edge(dgraph_t* graph, unsigned long first, unsigned long second) {
    struct edge* edge = edge_search(graph->edges, first, second);
    if(!edge) {
        edge = edge_create(first, second);
        LL_APPEND(graph->edges, edge);
        return true;
    }
    return false;
}
bool dot_set_attribute_on_edge(
    dgraph_t* graph,
    unsigned long first,
    unsigned long second,
    char* key,
    char* value) {
    struct edge* edge = edge_search(graph->edges, first, second);
    if(edge) {
        struct attribute** attrs = edge_attributes(edge);
        attribute_set(attrs, key, value);
        return true;
    } else return false;
}
char* dot_get_attribute_on_edge(
    dgraph_t* graph,
    unsigned long first,
    unsigned long second,
    char* key) {
    struct edge* edge = edge_search(graph->edges, first, second);
    if(edge) {
        struct attribute** attrs = edge_attributes(edge);
        return attribute_get(*attrs, key);
    } else return NULL;
}
bool dot_has_attribute_on_edge(
    dgraph_t* graph,
    unsigned long first,
    unsigned long second,
    char* key) {
    return dot_get_attribute_on_edge(graph, first, second, key) != NULL;
}
bool dot_remove_attribute_on_edge(
    dgraph_t* graph,
    unsigned long first,
    unsigned long second,
    char* key) {
    struct edge* edge = edge_search(graph->edges, first, second);
    if(edge) {
        struct attribute** attrs = edge_attributes(edge);
        return attribute_remove(attrs, key);
    } else return false;
}

#define DOT_PRINT_BUFFER_FUNC(func, ...)                                       \
    do {                                                                       \
        long _res = func(buffer + idx, buffer_size - idx, __VA_ARGS__);        \
        if(_res < 0 || _res > (long)(buffer_size - idx)) return -1;            \
        idx += _res;                                                           \
    } while(0)
#define DOT_PRINT_BUFFER(...) DOT_PRINT_BUFFER_FUNC(snprintf, __VA_ARGS__)

#define DOT_PRINT_INDENT                                                       \
    do {                                                                       \
        DOT_PRINT_BUFFER("%*s", (int)indent, "");                              \
    } while(0)

long graph_to_string(
    char* buffer,
    unsigned long buffer_size,
    dgraph_t* graph,
    unsigned long indent) {
    // keep track of how much of the buffer we have written to
    unsigned long idx = 0;

    DOT_PRINT_INDENT;
    bool directed = dot_is_directed(graph);
    if(directed && !dot_is_subgraph(graph)) {
        DOT_PRINT_BUFFER("di");
    }
    if(dot_is_subgraph(graph)) {
        DOT_PRINT_BUFFER("sub");
    }
    char name[MAX_ATTRIBUTE_LENGTH];
    if(dot_is_subgraph(graph)) {
        snprintf(name, MAX_ATTRIBUTE_LENGTH, "cluster_%p", graph);
    } else {
        snprintf(name, MAX_ATTRIBUTE_LENGTH, "\"%s\"", graph->name);
    }
    DOT_PRINT_BUFFER("graph %s {\n", name);

    if(!dot_is_subgraph(graph)) {
        snprintf(name, MAX_ATTRIBUTE_LENGTH, "%s", graph->name);
        for(size_t i = 0; i < MAX_ATTRIBUTE_LENGTH; i++)
            if(name[i] == ' ') name[i] = '_';
    }

    indent += 2;

    // extra buffers
    char prefix[16];
    char postfix[16];

    // print the graph attrs
    snprintf(prefix, 16, "%*s", (int)indent, "");
    snprintf(postfix, 16, ";\n");
    DOT_PRINT_BUFFER_FUNC(attributes_to_string, graph->attrs, prefix, postfix);

    // print the vertexes
    snprintf(prefix, 16, "[");
    snprintf(postfix, 16, "]");
    struct vertex* vertex;
    DL_FOREACH(graph->vertexes, vertex) {
        DOT_PRINT_INDENT;
        unsigned long id = vertex_id(vertex);
        struct attribute* attrs = *vertex_attributes(vertex);
        DOT_PRINT_BUFFER("%s_%lu", name, id);
        DOT_PRINT_BUFFER_FUNC(attributes_to_string, attrs, prefix, postfix);
        DOT_PRINT_BUFFER(";\n");
    }

    // print the edges
    snprintf(prefix, 16, "[");
    snprintf(postfix, 16, "]");
    struct edge* edge;
    DL_FOREACH(graph->edges, edge) {
        DOT_PRINT_INDENT;
        unsigned long id1 = edge_first_id(edge);
        unsigned long id2 = edge_second_id(edge);
        struct attribute* attrs = *edge_attributes(edge);
        DOT_PRINT_BUFFER(
            "%s_%lu%s%s_%lu",
            name,
            id1,
            directed ? "->" : "--",
            name,
            id2);
        DOT_PRINT_BUFFER_FUNC(attributes_to_string, attrs, prefix, postfix);
        DOT_PRINT_BUFFER(";\n");
    }

    dgraph_t* subgraph;
    DL_FOREACH2(graph->children, subgraph, children) {
        if(graph == subgraph->parent) // prevents extra nested subgraphs
            DOT_PRINT_BUFFER_FUNC(graph_to_string, subgraph, indent);
    }

    indent -= 2;
    DOT_PRINT_INDENT;
    DOT_PRINT_BUFFER("}\n");
    return idx;
}

long attributes_to_string(
    char* buffer,
    unsigned long buffer_size,
    struct attribute* attrs,
    char* prefix,
    char* postfix) {
    unsigned long idx = 0;
    struct attribute* attr;
    DL_FOREACH(attrs, attr) {
        char* key = attribute_key(attr);
        char* value = attribute_value(attr);
        DOT_PRINT_BUFFER(
            "%s%s=%s%s",
            prefix,
            key ? key : "",
            value ? value : "",
            postfix);
    }
    return idx;
}

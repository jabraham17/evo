#ifndef _DOT_DOT_H_
#define _DOT_DOT_H_

#include <stdbool.h>
#include <stddef.h>

struct dot_graph;

struct dot_graph* dot_create(char* name);
void dot_init(struct dot_graph* graph, char* name);
void dot_destroy(struct dot_graph* graph);

// returns true if successfully written
// returns false if we exceed the buffer size
bool dot_to_string(struct dot_graph* graph, char* buffer, size_t buffer_size);

void dot_set_directed(struct dot_graph* graph, bool directed);
bool dot_is_directed(struct dot_graph* graph);

bool dot_is_subgraph(struct dot_graph* graph);
void dot_add_subgraph(struct dot_graph* graph, struct dot_graph* subgraph);

void dot_set_attribute(struct dot_graph* graph, char* key, char* value);
char* dot_get_attribute(struct dot_graph* graph, char* key);
bool dot_has_attribute(struct dot_graph* graph, char* key);
bool dot_remove_attribute(struct dot_graph* graph, char* key);

bool dot_add_vertex(struct dot_graph* graph, unsigned long id);
bool dot_set_attribute_on_vertex(
    struct dot_graph* graph,
    unsigned long id,
    char* key,
    char* value);
char* dot_get_attribute_on_vertex(
    struct dot_graph* graph,
    unsigned long id,
    char* key);
bool dot_has_attribute_on_vertex(
    struct dot_graph* graph,
    unsigned long id,
    char* key);
bool dot_remove_attribute_on_vertex(
    struct dot_graph* graph,
    unsigned long id,
    char* key);

bool dot_add_edge(
    struct dot_graph* graph,
    unsigned long first,
    unsigned long second);
bool dot_set_attribute_on_edge(
    struct dot_graph* graph,
    unsigned long first,
    unsigned long second,
    char* key,
    char* value);
char* dot_get_attribute_on_edge(
    struct dot_graph* graph,
    unsigned long first,
    unsigned long second,
    char* key);
bool dot_has_attribute_on_edge(
    struct dot_graph* graph,
    unsigned long first,
    unsigned long second,
    char* key);
bool dot_remove_attribute_on_edge(
    struct dot_graph* graph,
    unsigned long first,
    unsigned long second,
    char* key);

#endif

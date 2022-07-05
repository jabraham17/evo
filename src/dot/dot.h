#ifndef _DOT_DOT_H_
#define _DOT_DOT_H_

#include <stdbool.h>
#include <stddef.h>

struct dot_graph;
typedef struct dot_graph dgraph_t;

dgraph_t* dot_create(char* name);
void dot_init(dgraph_t* graph, char* name);
void dot_destroy(dgraph_t* graph);

// returns true if successfully written
// returns false if we exceed the buffer size
bool dot_to_string(dgraph_t* graph, char* buffer, size_t buffer_size);

void dot_set_directed(dgraph_t* graph, bool directed);
bool dot_is_directed(dgraph_t* graph);

bool dot_is_subgraph(dgraph_t* graph);
void dot_add_subgraph(dgraph_t* graph, dgraph_t* subgraph);

void dot_set_attribute(dgraph_t* graph, char* key, char* value);
char* dot_get_attribute(dgraph_t* graph, char* key);
bool dot_has_attribute(dgraph_t* graph, char* key);
bool dot_remove_attribute(dgraph_t* graph, char* key);

bool dot_add_vertex(dgraph_t* graph, unsigned long id);
bool dot_set_attribute_on_vertex(
    dgraph_t* graph,
    unsigned long id,
    char* key,
    char* value);
char* dot_get_attribute_on_vertex(dgraph_t* graph, unsigned long id, char* key);
bool dot_has_attribute_on_vertex(dgraph_t* graph, unsigned long id, char* key);
bool dot_remove_attribute_on_vertex(
    dgraph_t* graph,
    unsigned long id,
    char* key);

bool dot_add_edge(dgraph_t* graph, unsigned long first, unsigned long second);
bool dot_set_attribute_on_edge(
    dgraph_t* graph,
    unsigned long first,
    unsigned long second,
    char* key,
    char* value);
char* dot_get_attribute_on_edge(
    dgraph_t* graph,
    unsigned long first,
    unsigned long second,
    char* key);
bool dot_has_attribute_on_edge(
    dgraph_t* graph,
    unsigned long first,
    unsigned long second,
    char* key);
bool dot_remove_attribute_on_edge(
    dgraph_t* graph,
    unsigned long first,
    unsigned long second,
    char* key);

#endif

#undef MAX_ATTRIBUTE_LENGTH
#define MAX_ATTRIBUTE_LENGTH 256

#define FN __attribute__((unused)) extern

//
// Attribute
//
struct attribute;
#ifndef DOT_PRIVATE_DEF_ATTRIBUTE_
    #define DOT_PRIVATE_DEF_ATTRIBUTE_
struct attribute {
    char key[MAX_ATTRIBUTE_LENGTH];
    char value[MAX_ATTRIBUTE_LENGTH];
    struct attribute* next;
};
#endif
FN struct attribute* attribute_create(char* key, char* value);
FN void attribute_init(struct attribute* attr, char* key, char* value);
FN void attribute_destroy_one(struct attribute* attr);
FN void attribute_destroy_all(struct attribute* attrs);

FN char* attribute_key(struct attribute* attr);
FN char* attribute_value(struct attribute* attr);

// operate on a list
// set and remove modify the attrs, so they need double ptr
FN void attribute_set(struct attribute** attrs, char* key, char* value);
FN char* attribute_get(struct attribute* attrs, char* key);
FN struct attribute* attribute_search(struct attribute* attrs, char* key);
FN int attribute_remove(struct attribute** attrs, char* key);

FN int attribute_cmp(struct attribute* a, struct attribute* b);

//
// Vertex
//
struct vertex;
#ifndef DOT_PRIVATE_DEF_VERTEX_
    #define DOT_PRIVATE_DEF_VERTEX_
struct vertex {
    unsigned long id;
    struct attribute* attrs;
    struct vertex* next;
};
#endif
FN struct vertex* vertex_create(unsigned long id);
FN void vertex_init(struct vertex* vertex, unsigned long id);
FN void vertex_destroy_one(struct vertex* vertex);
FN void vertex_destroy_all(struct vertex* vertexes);

FN unsigned long vertex_id(struct vertex* vertex);

// double ptr to allow for modification
FN struct attribute** vertex_attributes(struct vertex* vertex);

// operate on a list
FN struct vertex* vertex_search(struct vertex* vertexes, unsigned long id);

FN int vertex_cmp(struct vertex* a, struct vertex* b);

//
// Edge
//
struct edge;
#ifndef DOT_PRIVATE_DEF_EDGE_
    #define DOT_PRIVATE_DEF_EDGE_
struct edge {
    unsigned long first;
    unsigned long second;
    struct attribute* attrs;
    struct edge* next;
};
#endif
FN struct edge* edge_create(unsigned long first, unsigned long second);
FN void edge_init(struct edge* edge, unsigned long first, unsigned long second);
FN void edge_destroy_one(struct edge* edge);
FN void edge_destroy_all(struct edge* edges);

FN unsigned long edge_first_id(struct edge* edge);
FN unsigned long edge_second_id(struct edge* edge);

// double ptr to allow for modification
FN struct attribute** edge_attributes(struct edge* edge);

// operate on a list
FN struct edge*
edge_search(struct edge* edges, unsigned long first, unsigned long second);

FN int edge_cmp(struct edge* a, struct edge* b);

//
// String functions
//
FN long graph_to_string(
    char* buffer,
    unsigned long buffer_size,
    struct dot_graph* graph,
    unsigned long indent);
FN long attributes_to_string(
    char* buffer,
    unsigned long buffer_size,
    struct attribute* attrs,
    char* prefix,
    char* postfix);

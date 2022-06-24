#undef MAX_ATTRIBUTE_LENGTH
#define MAX_ATTRIBUTE_LENGTH 256

struct attribute;
static struct attribute* attribute_create(char* key, char* value);
static void attribute_init(struct attribute* attr, char* key, char* value);
static void attribute_destroy_one(struct attribute* attr);
static void attribute_destroy_all(struct attribute* attrs);

// operate on a list
static void attribute_set(struct attribute** attrs, char* key, char* value);
static char* attribute_get(struct attribute* attrs, char* key);
static struct attribute* attribute_search(struct attribute* attrs, char* key);
static int attribute_remove(struct attribute** attrs, char* key);

static int attribute_cmp(struct attribute* a, struct attribute* b);



struct vertex;
static struct vertex* vertex_create(unsigned long id);
static void vertex_init(struct vertex* vertex, unsigned long id);
static void vertex_destroy_one(struct vertex* vertex);
static vertex_destroy_all(struct vertex* vertexes);

static struct attribute** vertex_get_attributes(struct vertex* vertex);

// operate on a list
static struct vertex* vertex_search(struct vertex* vertexes, unsigned long id);

static int vertex_cmp(struct vertex* a, struct vertex* b);

struct edge;
static void edge_destroy_one(struct edge* edge);
static void edge_destroy_all(struct edge* edges);

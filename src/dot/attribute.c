#include "common/utlist.h"
#include "dot.h"
#include "private_defs.inc"
#include <stdlib.h>

struct attribute {
    char key[MAX_ATTRIBUTE_LENGTH];
    char value[MAX_ATTRIBUTE_LENGTH];
    struct attribute* next;
};

static struct attribute* attribute_create(char* key, char* value) {
    struct attribute* attr = malloc(sizeof(*attr));
    attribute_init(attr, key, value);
    return attr;
}
static void attribute_init(struct attribute* attr, char* key, char* value) {
    memset(attr, 0, sizeof(*attr));
    if(key) strncpy(attr->key, key, MAX_ATTRIBUTE_LENGTH - 1);
    if(value) strncpy(attr->value, value, MAX_ATTRIBUTE_LENGTH - 1);
}

static void attribute_destroy_one(struct attribute* attr) { free(attr); }
static void attribute_destroy_all(struct attribute* attrs) {
    struct attribute *attr_elm, *attr_tmp;
    LL_FOREACH_SAFE(attrs, attr_elm, attr_tmp) {
        LL_DELETE(attrs, attr_elm);
        attribute_destroy_one(attr_elm);
    }
}

static void attribute_set(struct attribute** attrs, char* key, char* value) {
    struct attribute* attr = attribute_search(*attrs, key);
    if(attr)
        if(value) strncpy(attr->value, value, MAX_ATTRIBUTE_LENGTH);
        else memset(elm->value, 0, MAX_ATTRIBUTE_LENGTH);
    else {
        attr = attribute_create(key, value);
        LL_APPEND(*attrs, attr);
    }
}
static char* attribute_get(struct attribute* attrs, char* key) {
    struct attribute* attr = attribute_search(attrs, key);
    return attr ? attr->value : NULL;
}
static struct attribute* attribute_search(struct attribute* attrs, char* key) {
    struct attribute attr;
    attribute_init(&attr, key, NULL);

    struct attribute* elm;
    LL_SEARCH(attrs, elm, &attr, attribute_cmp);

    return elm;
}
static int attribute_remove(struct attribute** attrs, char* key) {
    struct attribute* attr = attribute_search(*attrs, key);

    if(attr) {
        LL_DELETE(*attrs, attr);
        attribute_destroy_one(attr);
        return 1;
    } else return 0;
}

static int attribute_cmp(struct attribute* a, struct attribute* b) {
    return strncmp(a->key, b->key, MAX_ATTRIBUTE_LENGTH);
}

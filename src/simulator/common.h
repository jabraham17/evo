#ifndef SIMULATOR_COMMON_H_
#define SIMULATOR_COMMON_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "rand_help.h"

typedef struct {
    size_t x;
    size_t y;
} point_t;

typedef struct {
    point_t location;
    size_t width;
    size_t height;
} bounding_box_t;

#define COMMON_ATTR __attribute__((unused)) static

COMMON_ATTR bool point_equals(point_t* p1, point_t* p2) {
    return p1->x == p2->x && p1->y == p2->y;
}

COMMON_ATTR bool point_left(point_t* p1, point_t* p2) {
    return (p1->x - p2->x) == 1 && p1->y == p2->y;
}
COMMON_ATTR bool point_right(point_t* p1, point_t* p2) {
    return (p2->x - p1->x) == 1 && p1->y == p2->y;
}
COMMON_ATTR bool point_up(point_t* p1, point_t* p2) {
    return (p1->x - p2->x) == 1 && p1->x == p2->x;
}
COMMON_ATTR bool point_down(point_t* p1, point_t* p2) {
    return (p2->y - p1->y) == 1 && p1->x == p2->x;
}
COMMON_ATTR bool point_adjacent(point_t* p1, point_t* p2) {
    return point_left(p1, p2) || point_right(p1, p2) || point_up(p1, p2) || point_down(p1, p2);
}

COMMON_ATTR void point_random(point_t* p, size_t width, size_t height) {
    p->x = rand_max(width);
    p->y = rand_max(height);
}

#endif

#ifndef SIMULATOR_RAND_HELP_H_
#define SIMULATOR_RAND_HELP_H_
#include <stdlib.h>

// [0, n)
__attribute__((unused)) static int rand_max(int n) {

    int limit = RAND_MAX - (RAND_MAX % n);
    int r;
    while((r = rand()) >= limit)
        ;
    return r % n;
}
// [low, high]
__attribute__((unused)) static int rand_in_range_inclusive(int low, int high) {
    high += 1;
    int n = high - low;
    int r = rand_max(n);
    return low + r;
}
#endif

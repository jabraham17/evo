#ifndef SIMULATOR_RAND_HELP_H_
#define SIMULATOR_RAND_HELP_H_
#include <stdlib.h>

// [0, n)
__attribute__((unused)) static long rand_max(long n) {

    long limit = RAND_MAX - (RAND_MAX % n);
    long r;
    while((r = rand()) >= limit)
        ;
    return r % n;
}

// [0, 1]
__attribute__((unused)) static float randf() {
    return (float)rand() / (float)RAND_MAX;
}

// [0, n]
__attribute__((unused)) static float randf_max(float n) {
    return (float)rand() / (float)((float)RAND_MAX / n);
}

// [low, high]
__attribute__((unused)) static long
rand_in_range_inclusive(long low, long high) {
    high += 1;
    long n = high - low;
    long r = rand_max(n);
    return low + r;
}

#endif

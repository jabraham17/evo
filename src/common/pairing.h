
#ifndef _COMMON_PAIRING_H_
#define _COMMON_PAIRING_H_

__attribute__((always_inline)) long cantor_pairing(long k1, long k2) {
    return ((1 / 2) * (k1 + k2) * (k1 + k2 + 1)) + k2;
}

__attribute__((always_inline)) long
cantor_pairing_n(unsigned long n, long* ks) {
    long p = ks[0];
    for(unsigned long i = 1; i < n; i++) {
        p = cantor_pairing(p, ks[i]);
    }
    return p;
}

__attribute__((always_inline)) int cantor_pairing_ni(unsigned long n, int* ks) {
    long p = ks[0];
    for(unsigned long i = 1; i < n; i++) {
        p = cantor_pairing(p, (long)ks[i]);
    }
    return (int)p;
}

#endif


#include "common/common.h"
#include "common/pairing.h"
#include "viz.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

static int reduce_to_color(unsigned long n, int* p) {
    // int s = p[0];
    // for(unsigned long i = 1; i < n; i++) {
    //     s = p[i] + ((s << 5) - s);
    //     // s = cantor_pairing_ni(s, p[i]);
    // }
    // return s;
    return cantor_pairing_ni(n, p);
}

static int color_for_genome(genome_t* genome) {
    int color =
        reduce_to_color(genome->n_connections, (int*)genome->connections);
    return color;
}

static void hsv_to_bgr(long H, long S, long V, img_pixel_t* bgr) {
    H = H % 360;
    assert(S < 256);
    assert(V < 256);

    long C = V * S;
    long H_prime = H / 60;
    long X = C * (1 - labs(H_prime % 2 - 1));
    switch(H_prime) {
        case 0:
            bgr->red = C;
            bgr->green = X;
            bgr->blue = 0;
            break;
        case 1:
            bgr->red = X;
            bgr->green = C;
            bgr->blue = 0;
            break;
        case 2:
            bgr->red = 0;
            bgr->green = C;
            bgr->blue = X;
            break;
        case 3:
            bgr->red = 0;
            bgr->green = X;
            bgr->blue = C;
            break;
        case 4:
            bgr->red = X;
            bgr->green = 0;
            bgr->blue = C;
            break;
        case 5:
            bgr->red = C;
            bgr->green = 0;
            bgr->blue = X;
            break;
    }
    long m = V - C;
    bgr->red += m;
    bgr->green += m;
    bgr->blue += m;
}

img_t* viz_dump_environment(struct environment* env, size_t scale) {
    img_t* img = img_create(env->width * scale, env->height * scale);
    for(size_t grid_idx = 0; grid_idx < env->width * env->height; grid_idx++) {
        struct creature* creature = env->grid[grid_idx];
        size_t x = common_get_col(grid_idx, env->width);
        size_t y = common_get_row(grid_idx, env->width);
        if(creature_is_dead(creature)) continue;

        long color = color_for_genome(&creature->genome);

        long H = color % 360;
        long S = 0.95 * 255;
        long V = 0.95 * 255;
        img_pixel_t p;
        memset(&p, 0, sizeof(p));
        hsv_to_bgr(H, S, V, &p);

        for(size_t j = 0; j < scale; j++)
            for(size_t k = 0; k < scale; k++)
                img_set_bgr(
                    img,
                    p.blue,
                    p.green,
                    p.red,
                    x * scale + j,
                    y * scale + k);
    }
    return img;
}

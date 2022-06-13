
#include "viz.h"

img_t* viz_dump_environment(struct environment* env, size_t scale) {
    img_t* img = img_create(env->width*scale, env->height*scale);
    for(size_t i = 0; i < env->n_creatures; i++) {
        struct creature* creature = env->creatures[i];
        if(!creature) continue;

        for(size_t j = 0; j < scale; j++) 
        for(size_t k = 0; k < scale; k++) 
            img_set_bgr(img, 0x00, 0xFF, 0x00, creature->location.x*scale + j, creature->location.y*scale + k);
    }
    return img;
}


#include "viz.h"
#include "common/common.h"

img_t* viz_dump_environment(struct environment* env, size_t scale) {
    img_t* img = img_create(env->width * scale, env->height * scale);
    for(size_t grid_idx = 0; grid_idx < env->width * env->height; grid_idx++) {
        struct creature* creature = env->grid[grid_idx];
        size_t x = common_get_col(grid_idx, env->width);
        size_t y = common_get_row(grid_idx, env->width);
        if(creature_is_dead(creature)) continue;

        for(size_t j = 0; j < scale; j++)
            for(size_t k = 0; k < scale; k++)
                img_set_bgr(
                    img,
                    0x00,
                    0xFF,
                    0x00,
                    x * scale + j,
                    y * scale + k);
    }
    return img;
}


#include "viz/viz.h"

#include "img/img.h"
#include "simulator/environment.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <emscripten.h>

size_t canvas_width;
size_t canvas_height;
size_t canvas_scale;
size_t canvas_memory_bytes;
int32_t* canvas_memory = NULL;
EMSCRIPTEN_KEEPALIVE
void set_canvas_size(size_t width, size_t height, size_t scale) {
    canvas_width = width;
    canvas_height = height;
    canvas_scale = scale;
    canvas_memory_bytes = canvas_width * canvas_height * sizeof(*canvas_memory);
    if(canvas_memory != NULL) {
        free(canvas_memory);
    }
    canvas_memory =
        calloc(canvas_width * canvas_height, sizeof(*canvas_memory));
}
EMSCRIPTEN_KEEPALIVE
int32_t* get_canvas_memory() { return canvas_memory; }
EMSCRIPTEN_KEEPALIVE
size_t get_canvas_memory_nbytes() { return canvas_memory_bytes; }


void copy_img_to_canvas(img_t* img) {
    // srgb space
    for(size_t idx = 0; idx < img->height * img->width; idx++) {
        union {
            img_pixel_t p;
            uint32_t i;
        } x;
        x.p = img->pixels[idx];
        // srgb which is rgba, but because little endian its abgr
        canvas_memory[idx] = 0xFF << 24 |x.i;
    }
}

// EMSCRIPTEN_KEEPALIVE
// __attribute__((used))
extern void render(int32_t* memory, size_t nbytes);
// time_t last_render_time = 0;
// void call_render() {
//     // if(last_render_time != 0) {
//     //     time_t to_sleep = time(0) + 1 - last_render_time;
//     //     printf("tyime to sleep%d\n", to_sleep);
//     //     sleep(to_sleep);
//     // }
//     // last_render_time = time(0);
//     render();
// }

#define UNUSED __attribute__((unused))

#define ENV_DUMP_TO_CANVAS(ecd)                                                \
    do {                                                                     \
        img_t* img =                                                           \
            viz_dump_environment(ecd->env, ecd->env->args->output_scale);      \
        copy_img_to_canvas(img);                                              \
        img_destroy(img);                                                      \
        render(canvas_memory, canvas_memory_bytes); \
    } while(0)

void gen_start_callback(struct environment_callback_data* ecd) {
    ENV_DUMP_TO_CANVAS(ecd);
}
void gen_stop_callback(struct environment_callback_data* ecd) {
    ENV_DUMP_TO_CANVAS(ecd);
}
void gen_tick_callback(struct environment_callback_data* ecd) {
    ENV_DUMP_TO_CANVAS(ecd);
}
void gen_select_callback(struct environment_callback_data* ecd) {
    ENV_DUMP_TO_CANVAS(ecd);
}

EMSCRIPTEN_KEEPALIVE
int main(UNUSED int argc, UNUSED const char** argv) {

    struct environment_args env_args = ENVIRONMENT_DEFAULT_ARGS;
    env_args.n_ticks = 300;
    env_args.callback_end = 0;
    env_args.callback_select = 0;
    env_args.callback_tick = 1;
    env_args.callback_tick_freq = 10;

    printf("Seed: %ld\n", env_args.seed);
    srand(env_args.seed);

    env_args.width = canvas_width / canvas_scale;
    env_args.height = canvas_height / canvas_scale;
    env_args.output_scale = canvas_scale;

    printf("%ldx%ld\n", env_args.width, env_args.height);

    struct environment* env = environment_create(&env_args);
    environment_run_simulation(
        env,
        gen_start_callback,
        gen_tick_callback,
        gen_stop_callback,
        gen_select_callback);
    environment_destroy(env);
    printf("done\n");

    return 0;
}

#include "viz/viz.h"

#include "dot/dot.h"
#include "img/bmp.h"
#include "img/img.h"
#include "simulator/environment.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define UNUSED __attribute((unused))

#define BUFFER_MAX 512
char buffer[BUFFER_MAX];
char* bufptr;
size_t scale = 4;

void gen_start_callback(struct environment_callback_data* ecd) {
    img_t* img = viz_dump_environment(ecd->env, scale);
    bmp_t* bmp = bmp_create_from_img(img);
    sprintf(bufptr, "gen_%03zu_start.bmp", ecd->generation);
    bmp_write_to_file(bmp, buffer);
    bmp_destroy(bmp);
    img_destroy(img);
}
void gen_stop_callback(struct environment_callback_data* ecd) {
    img_t* img = viz_dump_environment(ecd->env, scale);
    bmp_t* bmp = bmp_create_from_img(img);
    sprintf(bufptr, "gen_%03zu_stop.bmp", ecd->generation);
    bmp_write_to_file(bmp, buffer);
    bmp_destroy(bmp);
    img_destroy(img);
}
void gen_tick_callback(struct environment_callback_data* ecd) {
    //if(ecd->microtick % 10 != 0) return;
    img_t* img = viz_dump_environment(ecd->env, scale);
    bmp_t* bmp = bmp_create_from_img(img);
    sprintf(
        bufptr,
        "gen_%03zu_tick_%04zu.bmp",
        ecd->generation,
        ecd->microtick);
    bmp_write_to_file(bmp, buffer);
    bmp_destroy(bmp);
    img_destroy(img);
}
void gen_select_callback(struct environment_callback_data* ecd) {
    img_t* img = viz_dump_environment(ecd->env, scale);
    bmp_t* bmp = bmp_create_from_img(img);
    sprintf(bufptr, "gen_%03zu_selection.bmp", ecd->generation);
    bmp_write_to_file(bmp, buffer);
    bmp_destroy(bmp);
    img_destroy(img);
}

int main(UNUSED int argc, UNUSED char** argv) {
    long seed = time(0);
    fprintf(stderr, "Seed: %ld\n", seed);
    srand(seed);

    #define OUTDIR_MAX 128
    char outdir[OUTDIR_MAX];
    if(argc < 2) strcpy(outdir, "output");
    else strncpy(outdir, argv[1], OUTDIR_MAX-1);
    struct stat st = {0};
    if(stat(outdir, &st) == -1) {
        mkdir(outdir, 0755);
    }
    bufptr = buffer;
    bufptr += sprintf(bufptr, "%s/", outdir);

    struct environment* env = environment_create(128, 128, 1000);
    environment_run_simulation(
        env,
        30,
        150,
        60,
        gen_start_callback,
        gen_tick_callback,
        gen_stop_callback,
        gen_select_callback);
    environment_destroy(env);

    return 0;
}

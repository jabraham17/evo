#include "viz/viz.h"

#include "dot/dot.h"
#include "img/bmp.h"
#include "img/img.h"
#include "simulator/environment.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define UNUSED __attribute((unused))

static void dump_c(struct creature* c, char* name) {
    dgraph_t* dg = viz_dump_creature(c);
    char buf[4096];
    if(dot_to_string(dg, buf, 4096)) {
        size_t buflen = strlen(buf);
        if(name != NULL) {
            char* filename = name;
            FILE* fp = fopen(filename, "w");
            fwrite(buf, buflen, 1, fp);
            fclose(fp);
        } else {
            fwrite(buf, buflen, 1, stdout);
        }
    } else {
        fprintf(stderr, "Failed to write dot\n");
    }
    dot_destroy(dg);
}

int main(UNUSED int argc, UNUSED char** argv) {
    long seed = time(0);
    fprintf(stderr, "Seed: %ld\n", seed);
    srand(seed);

    // struct dot_graph* dg = dot_create("hello");
    // dot_add_vertex(dg, 1);
    // dot_add_vertex(dg, 2);
    // dot_set_attribute(dg, "color", "blue");
    // dot_set_attribute_on_vertex(dg, 1, "color", "red");
    // dot_set_attribute_on_vertex(dg, 1, "label", "\"hello world\"");
    // dot_set_attribute_on_vertex(dg, 2, "label", "\"world\"");
    // dot_add_edge(dg, 1, 2);
    // // dot_set_attribute_on_edge(dg, 1, 2, "color", "green");

    // char buf[4096];
    // if(dot_to_string(dg, buf, 4096)) {
    //     FILE* fp = fopen("out.dot", "w");
    //     fwrite(buf, 4096, 1, fp);
    //     fclose(fp);
    // }

    // img_t* img = img_create(100, 100);
    // for(size_t i = 0; i < 100; i++) {
    //     for(size_t j = 0; j < 50; j++)
    //         img_set_bgr(img, 0xFF, 0x00, 0x00, i, j);
    //     for(size_t j = 50; j < 100; j++)
    //         img_set_bgr(img, 0x00, 0x00, 0xFF, i, j);
    // }

    // img_t* img = img_create(3, 2);
    // // RED GREEN BLUE
    // // BLUE GREEN RED
    // img_set_bgr(img, 0x00, 0x00, 0xFF, 0, 0);
    // img_set_bgr(img, 0x00, 0xFF, 0x00, 1, 0);
    // img_set_bgr(img, 0xFF, 0x00, 0x00, 2, 0);
    // img_set_bgr(img, 0xFF, 0x00, 0x00, 0, 1);
    // img_set_bgr(img, 0x00, 0xFF, 0x00, 1, 1);
    // img_set_bgr(img, 0x00, 0x00, 0xFF, 2, 1);

    // bmp_t* bmp = bmp_create_from_img(img);

    // bmp_write_to_file(bmp, "test.bmp");

    img_t* img;
    bmp_t* bmp;
    size_t scale = 2;

    struct environment* env = environment_create(256, 256);
    environment_add_creatures(env, 1000);
    environment_distribute(env);

    // img = viz_dump_environment(env, scale);
    // bmp = bmp_create_from_img(img);
    // bmp_write_to_file(bmp, "test.bmp");
    // bmp_destroy(bmp);
    // img_destroy(img);

    // dump_c(&env->creatures[0], "1.dot");
    // dump_c(&env->creatures[1], "2.dot");

    // return 0;

    char buf[20];

    size_t end = 1;
    for(size_t i = 1; i <= end; i++) {

        img = viz_dump_environment(env, scale);
        bmp = bmp_create_from_img(img);
        sprintf(buf, "%03zu_initial.bmp", i);
        bmp_write_to_file(bmp, buf);
        bmp_destroy(bmp);
        img_destroy(img);

        environment_run_generation(env, 300, 0);

        img = viz_dump_environment(env, scale);
        bmp = bmp_create_from_img(img);
        sprintf(buf, "%03zu_result.bmp", i);
        bmp_write_to_file(bmp, buf);
        bmp_destroy(bmp);
        img_destroy(img);

        environment_select(env, SELECTION_LEFT);

        img = viz_dump_environment(env, scale);
        bmp = bmp_create_from_img(img);
        sprintf(buf, "%03zu_select.bmp", i);
        bmp_write_to_file(bmp, buf);
        bmp_destroy(bmp);
        img_destroy(img);

        environment_next_generation(env);
    }

    environment_destroy(env);

    return 0;
}

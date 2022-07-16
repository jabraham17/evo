#include "viz/viz.h"

#include "dot/dot.h"
#include "img/bmp.h"
#include "img/img.h"
#include "simulator/environment.h"

#include "common/zlib_wrapper.h"
#include <popt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define UNUSED __attribute((unused))

#define BUFFER_MAX 512
static char buffer[BUFFER_MAX];

#define ENV_COMPRESS(file)                                                     \
    do {                                                                       \
        FILE* fp_in = fopen(file, "rb");                                       \
        strcpy(file + strlen(file), ".gz");                                    \
        FILE* fp_out = fopen(file, "wb");                                      \
        int ret = def(fp_in, fp_out, Z_DEFAULT_COMPRESSION);                   \
        if(ret != Z_OK) zerr(ret);                                             \
        fclose(fp_in);                                                         \
        fclose(fp_out);                                                        \
        file[strlen(file) - 3] = '\0';                                         \
        remove(file);                                                          \
    } while(0)

#define ENV_DUMP_TO_FILE(ecd, filename_addon, ...)                             \
    do {                                                                       \
        img_t* img =                                                           \
            viz_dump_environment(ecd->env, ecd->env->args->output_scale);      \
        bmp_t* bmp = bmp_create_from_img(img);                                 \
        sprintf(                                                               \
            buffer,                                                            \
            "%s/gen_%04zu_" filename_addon ".bmp",                             \
            ecd->env->args->output_dir,                                        \
            ecd->generation __VA_OPT__(, ) __VA_ARGS__);                       \
        bmp_write_to_file(bmp, buffer);                                        \
        bmp_destroy(bmp);                                                      \
        img_destroy(img);                                                      \
        /*ENV_COMPRESS(buffer);*/                                              \
    } while(0)

void gen_start_callback(struct environment_callback_data* ecd) {
    ENV_DUMP_TO_FILE(ecd, "start");
}
void gen_stop_callback(struct environment_callback_data* ecd) {
    ENV_DUMP_TO_FILE(ecd, "stop");
}
void gen_tick_callback(struct environment_callback_data* ecd) {
    ENV_DUMP_TO_FILE(ecd, "tick_%04zu", ecd->tick);
}
void gen_select_callback(struct environment_callback_data* ecd) {
    ENV_DUMP_TO_FILE(ecd, "select");
}

// emulate mkdir -p
void mkdirs(char* dirnames) {
    if(strlen(dirnames) == 0) return;
    char* dirname = strrchr(dirnames, '/');
    if(dirname) {
        size_t idx = dirname - dirnames;
        dirnames[idx] = '\0';
        mkdirs(dirnames);
        dirnames[idx] = '/';
    }
    struct stat st = {0};
    if(stat(dirnames, &st) == -1) {
        if(mkdir(dirnames, 0755) == -1) {
            fprintf(stderr, "Failed to create directory: %s\n", dirnames);
            exit(1);
        }
    }
}

int main(UNUSED int argc, UNUSED const char** argv) {

    struct environment_args env_args = {
        .seed = time(0),
        .output_dir = "output",
        .output_scale = 4,
        .callback_tick = 0,
        .callback_start = 1,
        .callback_end = 1,
        .callback_select = 0,
        .callback_tick_freq = 10,
        .width = 128,
        .height = 128,
        .n_creatures = 1000,
        .n_generations = 30,
        .n_ticks = 150,
        .threshold = 80,
        .n_connections = 10,
        .mutation_rate = 0.01,
#if defined(THREADED) && THREADED == 1
        .n_threads = 1
#endif
    };

    // https://linux.die.net/man/3/popt
    //  struct poptOption {
    //    const char* longName;
    //    char shortName;
    //    int argInfo;
    //    void* arg;
    //    int val;
    //    char* descrip;
    //    char* argDescrip;
    //  };
    //  POPT_ARGFLAG_SHOW_DEFAULT
    struct poptOption options[] = {
        {"seed",
         0,
         POPT_ARG_LONG,
         &env_args.seed,
         0,
         "seed the psuedo-random number generator (default: `time(0)`)",
         "SEED"},
        {"output-dir",
         'o',
         POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.output_dir,
         0,
         "directory to write output files to",
         "DIR"},
        {"scale",
         0,
         POPT_ARG_LONG | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.output_scale,
         0,
         "scale to write output at",
         "SCALE"},
        {"callback-tick",
         0,
         POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.callback_tick,
         0,
         "enable a callback per tick",
         "BOOL"},
        {"callback-start",
         0,
         POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.callback_start,
         0,
         "enable a callback at the start of a generation",
         "BOOL"},
        {"callback-end",
         0,
         POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.callback_end,
         0,
         "enable a callback at the end of a generation",
         "BOOL"},
        {"callback-select",
         0,
         POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.callback_select,
         0,
         "enable a callback after the selection of a generation",
         "BOOL"},
        {"callback-tick-freq",
         0,
         POPT_ARG_LONG | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.callback_tick_freq,
         0,
         "how often the tick callback fires if enabled",
         "FREQ"},
        {"width",
         'w',
         POPT_ARG_LONG | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.width,
         0,
         "width of the environment",
         "WIDTH"},
        {"height",
         'h',
         POPT_ARG_LONG | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.height,
         0,
         "height of the environment",
         "HEIGHT"},
        {"creatures",
         'n',
         POPT_ARG_LONG | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.n_creatures,
         0,
         "number of creatures to simulate",
         "N"},
        {"generations",
         'g',
         POPT_ARG_LONG | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.n_generations,
         0,
         "number of generations to simulate",
         "N"},
        {"ticks",
         't',
         POPT_ARG_LONG | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.n_ticks,
         0,
         "number of ticks per generation",
         "N"},
        {"threshold",
         0,
         POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.threshold,
         0,
         "gene expression threshold [-128, 127]",
         "T"},
        {"connections",
         'c',
         POPT_ARG_LONG | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.n_connections,
         0,
         "max number of gene connections",
         "N"},
        {"mutation-rate",
         'r',
         POPT_ARG_FLOAT | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.mutation_rate,
         0,
         "rate at which mutations occur [0.0, 1.0]",
         "RATE"},
#if defined(THREADED) && THREADED == 1
        {"threads",
         0,
         POPT_ARG_LONG | POPT_ARGFLAG_SHOW_DEFAULT,
         &env_args.n_threads,
         0,
         "number of threads to use (>0)",
         "N"},
#endif
        POPT_AUTOHELP POPT_TABLEEND
    };

    poptContext optCon = poptGetContext(argv[0], argc, argv, options, 0);
    char rc;
    while((rc = poptGetNextOpt(optCon)) >= 0) {
    }
    if(rc < -1) {
        fprintf(
            stderr,
            "Option Error on '%s': %s\n",
            poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
            poptStrerror(rc));
        exit(1);
    }

    fprintf(stderr, "Seed: %ld\n", env_args.seed);
    srand(env_args.seed);

    mkdirs(env_args.output_dir);

    struct environment* env = environment_create(&env_args);
    environment_run_simulation(
        env,
        gen_start_callback,
        gen_tick_callback,
        gen_stop_callback,
        gen_select_callback);
    environment_destroy(env);

    poptFreeContext(optCon);
    return 0;
}

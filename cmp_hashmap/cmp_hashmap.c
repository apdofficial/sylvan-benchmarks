#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "sylvan_int.h"

static size_t nworkers = 4;
static size_t nrounds = 10;
static size_t samples_per_round = 100; // expected max

void sylvan_setup(uint64_t memoryCap)
{
    sylvan_set_limits(memoryCap, 1, 2);
    sylvan_init_package();
    sylvan_init_mtbdd();
    sylvan_gc_disable();
}

static double wctime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec + 1E-6 * tv.tv_usec);
}

static inline double wctime_sec_elapsed(double t_start)
{
    return wctime() - t_start;
}

static inline double wctime_ms_elapsed(double start)
{
    return wctime_sec_elapsed(start) * 1000;
}

VOID_TASK_2(create_variables, size_t, start, size_t, end)
{
    for (; start < end; ++start) {
        MTBDD v = mtbdd_ithvar(start);
        if (v == mtbdd_invalid) {
            printf("table is full\n");
            break;
        }
    }
}

TASK_0(int, run)
{
    double runtimes[nrounds][samples_per_round];
    double usages[nrounds][samples_per_round];

    for (size_t round = 0; round < nrounds; ++round){
        printf("round %zu\n", round);
        sylvan_setup(4LL*1024LLU*1024LLU*1024LLU);
        size_t sample = 0;
        size_t step = 200000 / nworkers;

        for (size_t index = 0; index < 50000000;) {
            double start = wctime();

            for (size_t i = 0; i < nworkers; ++i){
                SPAWN(create_variables, index, index+step);
                index += step;
            }

            for (size_t i = 0; i < nworkers; ++i) SYNC(create_variables);

            double runtime = wctime_ms_elapsed(start);

            size_t filled, total;
            sylvan_table_usage(&filled, &total);
            double usage = ((double)filled/(double)total)*100;

//            printf("r %zu | s %zu | table usage %.2f%% | runtime: %.2fns\n", round, sample, usage, runtime);

#if SYLVAN_USE_LINEAR_PROBING
            if (usage <= 0 || usage >= 96.5) break;
#else
            if (usage <= 0 || usage >= 97.7) break;
#endif

            if (runtime <= 0 || runtime >= 1000) break;

            if (round == 0) continue; // warm up round
            if (sample > samples_per_round) break;

            usages[round-1][sample] = usage;
            runtimes[round-1][sample] = runtime;
            sample++;
        }

        sylvan_quit();
    }

    char filename[100];

#if SYLVAN_USE_LINEAR_PROBING
    sprintf(filename,   "./w%zu_probing.csv", nworkers);
#else
    sprintf(filename,   "./w%zu_chaining.csv", nworkers);
#endif

    FILE *file = fopen(filename, "w+");
    if(!file) return EXIT_FAILURE;

    fprintf(file, "round,table_usage,runtime_ms\n");
    for (size_t round = 0; round < nrounds; round++){
        for (size_t sample = 0; sample < samples_per_round; sample++) {
            if (usages[round][sample] <= 0 || usages[round][sample] >= 100) break;
            if (runtimes[round][sample] <= 0 || runtimes[round][sample] >= 1000) break;
            fprintf(file, "%zu,%.2f,%.2f\n", round, usages[round][sample], runtimes[round][sample]);
        }
    }
    fclose(file);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
#if SYLVAN_USE_LINEAR_PROBING
    printf("running probing\n");
#else
    printf("running chaining\n");
#endif

    size_t num_tests = nworkers;
    for (size_t i = 1; i <= num_tests; ++i) {
        nworkers = i;
        lace_start(nworkers, 10000000);
        printf("running with %zu worker(s)\n", i);
        int res = RUN(run);
        lace_stop();
        if (res == EXIT_FAILURE) return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
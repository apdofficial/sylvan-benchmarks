#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sylvan_int.h"

static size_t workers = 4;
static size_t rounds = 10;
static size_t samples_per_round = 100; // expected max

void sylvan_setup(uint64_t memoryCap)
{
    sylvan_set_limits(memoryCap, 1, 2);
    sylvan_init_package();
    sylvan_init_mtbdd();
    sylvan_gc_disable();
}

void set_filename(char* filename, size_t len)
{
    assert(len >= 100);
#if SYLVAN_USE_CHAINING
    sprintf(filename,   "./w%zu_chaining.csv", workers);
#elif SYLVAN_USE_LIMITED_PROBE_SEQUENCE
    sprintf(filename,   "./w%zu_probing_limited.csv", workers);
#else
    sprintf(filename,   "./w%zu_probing_unlimited.csv", workers);
#endif
}

// number of variables created per LACE task
size_t get_step_size()
{
#if SYLVAN_USE_CHAINING
        return 100000 / workers;
#elif SYLVAN_USE_LIMITED_PROBE_SEQUENCE
        return 100000 / workers;
#else
        return 100000 / workers;
#endif
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
    float runtimes[rounds][samples_per_round];
    float usages[rounds][samples_per_round];

    for (size_t round = 0; round < rounds; ++round){
        printf("round %zu\n", round);
        sylvan_setup(2LL*1024LLU*1024LLU*1024LLU);
        size_t sample = 0;
        size_t step = get_step_size();

        for (size_t index = 0; index < 50000000;) {
            clock_t start = clock();

            for (size_t i = 0; i < workers; ++i){
                SPAWN(create_variables, index, index+step);
                index += step;
            }

            for (size_t i = 0; i < workers; ++i) SYNC(create_variables);

            float runtime = clock_ms_elapsed(start);

            float used = llmsset_count_marked(nodes);
            float all = llmsset_get_size(nodes);
            float usage = (used/all)*100;
            if (usage <= 0 || usage >= 97.7) break;
            if (runtime <= 0 || runtime >= 1000) break;

            if (round == 0) continue; // warm up round
            if (sample > samples_per_round) break;

//            printf("r %zu | s %zu | table usage %.2f%% | runtime: %.2fns\n", round, sample, usage, runtime);

            usages[round-1][sample] = usage;
            runtimes[round-1][sample] = runtime;
            sample++;
        }

        sylvan_quit();
    }

    size_t len = 100;
    char filename[len];

    set_filename(filename, len);

    // write the raw data into a csv
    FILE *file;
    file = fopen(filename, "w+");
    fprintf(file, "round,table_usage,runtime_ns\n");
    for (size_t round = 0; round < rounds; round++){
        for (size_t sample = 0; sample < samples_per_round; sample++) {
            if (usages[round][sample] <= 0 || usages[round][sample] >= 97.7) break;
            if (runtimes[round][sample] <= 0 || runtimes[round][sample] >= 1000) break;
            fprintf(file, "%zu,%.2f,%.2f\n", round, usages[round][sample], runtimes[round][sample]);
        }
    }
    fclose(file);

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    lace_start(workers, 10000000);
    sylvan_gc_disable();

    size_t num_tests = workers + 1;
    for (size_t i = 1; i < num_tests; ++i) {
        workers = i;
        printf("running with %zu worker(s)\n", i);
        RUN(run);
    }

    lace_stop();
    return EXIT_SUCCESS;
}
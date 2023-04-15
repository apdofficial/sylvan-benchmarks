/*
 * Copyright 2016 Tom van Dijk, Johannes Kepler University Linz
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sylvan_int.h>
#include <sys/time.h>
#include "sylvan_varswap.h"
#include "sylvan_levels.h"
#include "sylvan_reorder.h"

/**
 * Block size tunes the granularity of the parallel distribution
 */
#define BLOCKSIZE 128

#define STATS 0 // useful information w.r.t. dynamic reordering

static int reorder_initialized = 0;

struct sifting_config
{
    reorder_termination_cb termination_cb;      // termination callback
    double t_start_sifting;                     // start time of the sifting
    uint32_t threshold;                         // threshold for number of nodes per level
    float max_growth;                           // coefficient used to calculate maximum growth
    uint32_t max_swap;                          // maximum number of swaps per sifting
    uint32_t total_num_swap;                    // number of swaps completed
    uint32_t max_var;                           // maximum number of vars swapped per sifting
    uint32_t total_num_var;                     // number of vars sifted
    double time_limit_ms;                       // time limit in milliseconds
};

/// reordering configurations
static struct sifting_config configs = {
        .termination_cb = NULL,
        .t_start_sifting = 0,
        .threshold = 64,
        .max_growth = 1.1f,
        .max_swap = 10000,
        .total_num_swap = 0,
        .max_var = 2000,
        .total_num_var = 0,
        .time_limit_ms = 1 * 60 * 1000 // 1 minute
};

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

static int should_terminate_reordering(const struct sifting_config *reorder_config);

void sylvan_init_reorder()
{
    if (reorder_initialized) return;
    reorder_initialized = 1;

    sylvan_init_mtbdd();

    sylvan_register_quit(&sylvan_quit_reorder);
    mtbdd_levels_gc_add_mark_managed_refs();
}

void sylvan_quit_reorder()
{
    mtbdd_resetlevels();
    reorder_initialized = 0;
}

reorder_config_t sylvan_get_reorder_config()
{
    return (reorder_config_t) &configs;
}

void sylvan_set_reorder_terminationcb(reorder_termination_cb callback)
{
    configs.termination_cb = callback;
}

void sylvan_set_reorder_threshold(uint32_t threshold)
{
    configs.threshold = threshold;
}

void sylvan_set_reorder_maxgrowth(float max_growth)
{
    assert(max_growth > 1.0f);
    configs.max_growth = max_growth;
}

void sylvan_set_reorder_maxswap(uint32_t max_swap)
{
    configs.max_swap = max_swap;
}

void sylvan_set_reorder_maxvar(uint32_t max_var)
{
    configs.max_var = max_var;
}

void sylvan_set_reorder_timelimit(double time_limit)
{
    configs.time_limit_ms = time_limit;
}

static inline void update_best_pos(sifting_state_t *state)
{
    state->size = llmsset_count_marked(nodes);
    if (state->size < state->best_size) {
        state->best_size = state->size;
        state->best_pos = state->pos;
    }
}

static inline int is_max_growth_reached(const sifting_state_t *state)
{
    return ((float) (state->size) >= configs.max_growth * (float) (state->best_size));
}

TASK_IMPL_1(varswap_t, sylvan_siftdown, sifting_state_t*, state)
{
    for (; state->pos < state->high; ++state->pos) {
        varswap_t res = sylvan_varswap(state->pos);
        if (!sylvan_varswap_issuccess(res)) return res;
        configs.total_num_swap++;
        update_best_pos(state);
        if (is_max_growth_reached(state)) {
            ++state->pos;
            break;
        }
    }
    return SYLVAN_VARSWAP_SUCCESS;
}


TASK_IMPL_1(varswap_t, sylvan_siftup, sifting_state_t*, state)
{
    for (; state->pos > state->low; --state->pos) {
        varswap_t res = sylvan_varswap(state->pos - 1);
        if (!sylvan_varswap_issuccess(res)) return res;
        configs.total_num_swap++;
        update_best_pos(state);
        if (is_max_growth_reached(state)) {
            --state->pos;
            break;
        }
    }
    return SYLVAN_VARSWAP_SUCCESS;
}

TASK_IMPL_2(varswap_t, sylvan_siftpos, BDDLABEL, pos, BDDLABEL, target)
{
    for (; pos < target; pos++) {
        varswap_t res = sylvan_varswap(pos);
        if (!sylvan_varswap_issuccess(res)) return res;
        configs.total_num_swap++;
    }
    for (; pos > target; pos--) {
        varswap_t res = sylvan_varswap(pos - 1);
        if (!sylvan_varswap_issuccess(res)) return res;
        configs.total_num_swap++;
    }
    return SYLVAN_VARSWAP_SUCCESS;
}


TASK_IMPL_1(varswap_t, sylvan_reorder_perm, const uint32_t*, permutation)
{
    sylvan_gc();
    sylvan_gc_disable();

    varswap_t res = SYLVAN_VARSWAP_SUCCESS;
    int identity = 1;

    // check if permutation is identity
    for (size_t level = 0; level < mtbdd_levelscount(); level++) {
        if (permutation[level] != mtbdd_level_to_var(level)) {
            identity = 0;
            break;
        }
    }
    if (identity) return res;

    for (size_t level = 0; level < mtbdd_levelscount(); ++level) {
        BDDLABEL var = permutation[level];
        BDDLABEL pos = mtbdd_var_to_level(var);
        res = sylvan_siftpos(pos, level);
        if (!sylvan_varswap_issuccess(res)) break;
    }

    sylvan_gc_enable();
    sylvan_gc();

    return res;
}

TASK_IMPL_2(varswap_t, sylvan_reorder, BDDLABEL, low, BDDLABEL, high)
{
    sylvan_gc();
    sylvan_gc_disable();

    if (mtbdd_levelscount() < 1) return SYLVAN_VARSWAP_ERROR;

    configs.t_start_sifting = wctime();
    configs.total_num_swap = 0;
    configs.total_num_var = 0;

    // if high == 0, then we sift all variables
    if (high == 0) high = mtbdd_levelscount() - 1;

#if STATS
    printf("Sifting between variable labels %d and %d\n", low, high);
    size_t before_size = llmsset_count_marked(nodes);
#endif

    // now count all variable levels (parallel...)
    size_t level_counts[mtbdd_levelscount()];
    for (size_t i = 0; i < mtbdd_levelscount(); i++) level_counts[i] = 0;
    sylvan_count_nodes(level_counts);

    // mark and sort
    int levels[mtbdd_levelscount()];
    mtbdd_mark_threshold(levels, level_counts, configs.threshold);
    gnome_sort(levels, level_counts);

    varswap_t res;
    sifting_state_t state;
    state.low = low;
    state.high = high;
    state.size = llmsset_count_marked(nodes);
    state.best_size = state.size;

    for (size_t i = 0; i < mtbdd_levelscount(); i++) {
        if (levels[i] < 0) break; // marked level, done
        uint64_t lvl = levels[i];

        state.pos = mtbdd_level_to_var(lvl);
        if (state.pos < low || state.pos > high) continue; // skip, not in range

        state.best_pos = state.pos;

        // search for the optimum variable position
        // first sift to the closest boundary, then sift in the other direction
        if (lvl > mtbdd_levelscount() / 2) {
            res = sylvan_siftdown(&state);
            if (!sylvan_varswap_issuccess(res)) break;
            res = sylvan_siftup(&state);
            if (!sylvan_varswap_issuccess(res)) break;
        } else {
            res = sylvan_siftup(&state);
            if (!sylvan_varswap_issuccess(res)) break;
            res = sylvan_siftdown(&state);
            if (!sylvan_varswap_issuccess(res)) break;
        }

        // optimum variable position restoration
        res = sylvan_siftpos(state.pos, state.best_pos);
        if (!sylvan_varswap_issuccess(res)) break;

        configs.total_num_var++;
        if (should_terminate_reordering(&configs)) break;
#if STATS
        if (state.best_size < state.size)
            printf("Reduced the number of nodes from %zu to %zu\n", state.size, state.best_size);
#endif
    }

#if STATS
    size_t after_size = llmsset_count_marked(nodes);
    printf("Reordering reduced the number of nodes from %zu to %zu\n", before_size, after_size);
#endif

    sylvan_gc_enable();
    sylvan_gc();

    return res;
}

static int should_terminate_reordering(const struct sifting_config *reorder_config)
{
    if (reorder_config->termination_cb != NULL && reorder_config->termination_cb()) {
#if STATS
        printf("sifting exit: termination_cb\n");
#endif
        return 1;
    }
    if (reorder_config->total_num_swap > reorder_config->max_swap) {
#if STATS
        printf("sifting exit: reached %u from the total_num_swap %u\n",
               reorder_config->total_num_swap,
               reorder_config->max_swap);
#endif
        return 1;
    }
    if (reorder_config->total_num_var > reorder_config->max_var) {
#if STATS
        printf("sifting exit: reached %u from the total_num_var %u\n",
               reorder_config->total_num_var,
               reorder_config->max_var);
#endif
        return 1;
    }
    double t_elapsed = wctime_ms_elapsed(reorder_config->t_start_sifting);
    if (t_elapsed > reorder_config->time_limit_ms) {
#if STATS
        printf("sifting exit: reached %fms from the time_limit %.2fms\n",
               t_elapsed,
               reorder_config->time_limit_ms);
#endif
        return 1;
    }
    return 0;
}
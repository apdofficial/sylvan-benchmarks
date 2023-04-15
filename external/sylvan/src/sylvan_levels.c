#include "sylvan_int.h"
#include "sylvan_levels.h"
#include "sylvan_mtbdd_int.h"

/// Handling of variable levels
static BDDLEVEL *level_to_var = NULL;   // get the level of a "real variable"
static BDDVAR *var_to_level = NULL;   // get the variable label of a level
static MTBDD *levels = NULL;            // array holding the 1-node BDD for each level
static size_t levels_count = 0;         // number of created levels
static size_t levels_size = 0;          // size of the 3 arrays


MTBDD mtbdd_newlevel(void)
{
    mtbdd_newlevels(1);
    return levels[levels_count - 1];
}

int mtbdd_newlevels(size_t amount)
{
    if (levels_count + amount >= levels_size) {
#if 0
        if (levels_size == 0) levels_size = 1; // start here
        while (levels_count + amount >= levels_size) levels_size *= 2;
#else
        // just round up to the next multiple of 64 value
        // probably better than doubling anyhow...
        levels_size = (levels_count + amount + 63) & (~63LL);
#endif
        levels = realloc(levels, sizeof(MTBDD[levels_size]));
        level_to_var = realloc(level_to_var, sizeof(BDDLEVEL[levels_size]));
        var_to_level = realloc(var_to_level, sizeof(BDDVAR[levels_size]));

        if (!(levels && level_to_var && var_to_level)) {
            fprintf(stderr, "mtbdd_newlevels failed to allocate new memory!");
            return 0;
        }
    }
    for (size_t i = 0; i < amount; i++) {
        // reminder: makenode(var, low, high)
        levels[levels_count] = mtbdd_makenode(levels_count, mtbdd_false, mtbdd_true);
        level_to_var[levels_count] = levels_count;
        var_to_level[levels_count] = levels_count;
        levels_count++;
    }
    return 1;
}

void mtbdd_resetlevels(void)
{
    if (levels_size != 0) {
        free(levels);
        levels = NULL;
        free(level_to_var);
        level_to_var = NULL;
        free(var_to_level);
        var_to_level = NULL;
        levels_count = 0;
        levels_size = 0;
    }
}

size_t mtbdd_levelscount(void)
{
    return levels_count;
}

MTBDD mtbdd_ithlevel(BDDLEVEL level)
{
    if (level < levels_count) return levels[level_to_var[level]];
    else return mtbdd_invalid;
}

BDDLEVEL mtbdd_var_to_level(BDDVAR var)
{
    if (var < levels_count) return var_to_level[var];
    else return var;
}

BDDVAR mtbdd_level_to_var(BDDLEVEL level)
{
    if (level < levels_count) return level_to_var[level];
    else return level;
}

/**
 * This function is called during garbage collection and
 * marks all managed level BDDs so they are kept.
 */
VOID_TASK_0(mtbdd_gc_mark_managed_refs)
{
    for (size_t i = 0; i < levels_count; i++) {
        llmsset_mark(nodes, MTBDD_STRIPMARK(levels[i]));
    }
}

void mtbdd_levels_gc_add_mark_managed_refs(void)
{
    sylvan_gc_add_mark(TASK(mtbdd_gc_mark_managed_refs));
}

void mtbdd_varswap(BDDVAR var)
{
    var_to_level[level_to_var[var]] = var + 1;
    var_to_level[level_to_var[var + 1]] = var;
    uint32_t save = level_to_var[var];
    level_to_var[var] = level_to_var[var + 1];
    level_to_var[var + 1] = save;
}

/**
 * Sort level counts using gnome sort.
 * @param level
 * @param level_counts
 */
void gnome_sort(int *levels_arr, const size_t *level_counts)
{
    unsigned int i = 1;
    unsigned int j = 2;
    while (i < mtbdd_levelscount()) {
        long p = levels_arr[i - 1] == -1 ? -1 : (long) level_counts[mtbdd_level_to_var(levels_arr[i - 1])];
        long q = levels_arr[i] == -1 ? -1 : (long) level_counts[mtbdd_level_to_var(levels_arr[i])];
        if (p < q) {
            int t = levels_arr[i];
            levels_arr[i] = levels_arr[i - 1];
            levels_arr[i - 1] = t;
            if (--i) continue;
        }
        i = j++;
    }
}

VOID_TASK_IMPL_3(sylvan_count_nodes, size_t*, arr, size_t, first, size_t, count)
{
    // Divide-and-conquer if count above COUNT_NODES_BLOCK_SIZE
    if (count > COUNT_NODES_BLOCK_SIZE) {
        SPAWN(sylvan_count_nodes, arr, first, count / 2);
        CALL(sylvan_count_nodes, arr, first + count / 2, count - count / 2);
        SYNC(sylvan_count_nodes);
    } else {
        size_t tmp[mtbdd_levelscount()];
        size_t i;
        for (i = 0; i < mtbdd_levelscount(); i++) tmp[i] = 0;

        const size_t end = first + count;

        for (; first < end; first++) {
            if (!llmsset_is_marked(nodes, first)) continue; // unused bucket
            mtbddnode_t node = MTBDD_GETNODE(first);
            if (mtbddnode_isleaf(node)) continue; // a leaf
            tmp[mtbddnode_getvariable(node)]++; // update the variable
        }
        /* these are atomic operations on a hot location with false sharing inside another
           thread's program stack... can't get much worse! */
        for (i = 0; i < mtbdd_levelscount(); i++) __sync_add_and_fetch(&arr[i], tmp[i]);
    }
}

// set levels below the threshold to -1
void mtbdd_mark_threshold(int *level, const size_t *level_counts, uint32_t threshold)
{
    for (unsigned int i = 0; i < levels_count; i++) {
        if (level_counts[mtbdd_level_to_var(i)] < threshold) level[i] = -1;
        else level[i] = i;
    }
}
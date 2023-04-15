#ifndef SYLVAN_SYLVAN_LEVELS_H
#define SYLVAN_SYLVAN_LEVELS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * When using dynamic variable reordering, it is strongly recommended to use
 * "levels" instead of working directly with the internal variables.
 *
 * Dynamic variable reordering requires that variables are consecutive.
 * Initially, variables are assigned linearly, starting with 0.
 */

/**
 * 4096, because that is not very small, and not very large
 * typical kind of parameter that is open to tweaking, though I don't expect it matters so much
 * too small is bad for the atomic operations, too large is bad for work-stealing
 * with 2^20 - 2^25 nodes table size, this is 256 - 8192 tasks
 */
#define COUNT_NODES_BLOCK_SIZE 4096

VOID_TASK_DECL_3(sylvan_count_nodes, size_t*, size_t, size_t);
/**
 * @brief Count the number of nodes per real variable level in parallel.
 * @details Results are stored atomically in arr. To make this somewhat scalable, we use a
 * standard binary reduction pattern with local arrays...
 * Fortunately, we only do this once per call to dynamic variable reordering.
 * \param level_counts array into which the result is stored
 */
#define sylvan_count_nodes(level_counts) RUN(sylvan_count_nodes, level_counts, 0, nodes->table_size)

/**
 * @brief Create the next level and return the BDD representing the variable (ithlevel)
 * @details The BDDs representing managed levels are always kept during garbage collection.
 * NOTE: not currently thread-safe.
 */
MTBDD mtbdd_newlevel(void);

/**
 * @brief Create the next <amount> levels
 * @details The BDDs representing managed levels are always kept during garbage collection. Not currently thread-safe.
 * \param amount number of levels to create
 */
int mtbdd_newlevels(size_t amount);

/**
 * \brief  Reset all levels.
 */
void mtbdd_resetlevels(void);

/**
 * \brief  Get the BDD node representing "if level then true else false"
 * \details  Order a node does not change after a swap, meaning it is in the same level,
 * however, after a swap they can point to a different variable
 * \param level for which the BDD needs to be returned
 */
MTBDD mtbdd_ithlevel(BDDLEVEL level);

/**
 * @brief  Get the level of the given variable
 */
BDDLEVEL mtbdd_var_to_level(BDDVAR var);

/**
 * @brief  Get the variable of the given level
 */
BDDVAR mtbdd_level_to_var(BDDLEVEL level);

/**
 * \brief  Get the number of created levels
 */
size_t mtbdd_levelscount(void);

/**
 * \brief  Return the level of the given internal node.
 * \param node for which the level needs to be returned
 */
BDDLEVEL mtbdd_node_to_level(MTBDD node);

/**
 * \brief  Add callback to mark managed references during garbage collection.
 * \details This is used for the dynamic variable reordering.
 */
void mtbdd_levels_gc_add_mark_managed_refs(void);

/**
 * \brief Swap the levels of two variables var and var+1
 * \details This is used for the dynamic variable reordering.
 * <ul>
 * <li>swap the level_to_var of var and var+1
 * <li>swap the var_to_level of level var and level var+1
 * </ul>
 * \param var variable to be swapped with var+1
 */
void mtbdd_varswap(BDDVAR var);

/**
 * @brief  Mark all nodes that are reachable from the given level.
 */
void mtbdd_mark_threshold(int* level, const size_t* level_counts, uint32_t threshold);

/**
 * @brief  Sort the levels in descending order according to the number of nodes.
 */
void gnome_sort(int *levels, const size_t *level_counts);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //SYLVAN_SYLVAN_LEVELS_H

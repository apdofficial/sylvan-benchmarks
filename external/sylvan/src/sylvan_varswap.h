#ifndef SYLVAN_SYLVAN_VAR_SWAP_H
#define SYLVAN_SYLVAN_VAR_SWAP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Block size tunes the granularity of the parallel distribution
 */
#define BLOCKSIZE 128

typedef enum varswap {
    /// the operation was aborted and rolled back
    SYLVAN_VARSWAP_ROLLBACK = 1,
    /// success
    SYLVAN_VARSWAP_SUCCESS = 0,
    //// cannot rehash in phase 1, no marked nodes remaining
    SYLVAN_VARSWAP_P0_CLEAR_FAIL = -1,
    //// cannot rehash in phase 1, no marked nodes remaining
    SYLVAN_VARSWAP_P1_REHASH_FAIL = -2,
    /// cannot rehash in phase 1, and marked nodes remaining
    SYLVAN_VARSWAP_P1_REHASH_FAIL_MARKED = -3,
    /// cannot rehash in phase 2, no marked nodes remaining
    SYLVAN_VARSWAP_P2_REHASH_FAIL = -4,
    /// cannot create node in phase 2 (ergo marked nodes remaining)
    SYLVAN_VARSWAP_P2_CREATE_FAIL = -5,
    /// cannot rehash and cannot create node in phase 2
    SYLVAN_VARSWAP_P2_REHASH_AND_CREATE_FAIL = -6,
    /// the operation was aborted and rolled back
    SYLVAN_VARSWAP_ERROR = -7,
} varswap_t;

/**
 * @brief Provide description for given result.
 *
 * @details Requires buffer with length at least equal to 100
 *
 * @param tag
 * @param result based on which the description is determined
 * @param buf buffer into which the description will be copied
 * @param buf_len
 */
void sylvan_varswap_resdescription(varswap_t result, char *buf, size_t buf_len);

static inline int sylvan_varswap_issuccess(varswap_t result)
{
    return result == SYLVAN_VARSWAP_SUCCESS || result == SYLVAN_VARSWAP_ROLLBACK;
}

void sylvan_print_varswap_res(varswap_t result);

TASK_DECL_1(varswap_t, sylvan_varswap, BDDLABEL);
 /**
  * @brief Swaps two consecutive variables in the entire forest.
  *
  * Variable swapping consists of two phases. The first phase performs
  * variable swapping on all simple cases. The cases that require node
  * lookups are left marked. The second phase then fixes the marked nodes.
  *
  * If the "recovery" parameter is set, then phase 1 only rehashes nodes that are <var+1>,
  * and phase 2 will not abort on the first error, but try to finish as many nodes as possible.
  *
  * We assume there are only BDD/MTBDD/MAP nodes in the forest.
  * The operation is not thread-safe, so make sure no other Sylvan operations are
  * done when performing variable swapping.
  *
  * Variable swapping consists of two phases. The first phase performs
  * variable swapping on all simple cases. The cases that require node
  * lookups are left marked. The second phase then fixes the marked nodes.
  *
  * It is recommended to clear the cache and perform clear-and-mark (the first part of garbage
  * collection, before resizing and rehashing) before running sylvan_varswap.
  *
  * If the parameter <recovery> is set, then phase 1 only rehashes nodes that have variable "var+1".
  * Phase 2 will not abort on the first error, but try to finish as many nodes as possible.
  *
  * Return varswap_res_t
  *
  * See the implementation of sylvan_simple_varswap for notes on recovery/rollback.
  *
  * Due to the nature of bounded probe sequences, it is possible that rehashing
  * fails, even when nothing is changed. Worst case: recovery impossible.
  *
  * @param var variable to swap
  * @return varswap_res_t
  *
  */
#define sylvan_varswap(pos) CALL(sylvan_varswap, pos)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //SYLVAN_SYLVAN_VAR_SWAP_H

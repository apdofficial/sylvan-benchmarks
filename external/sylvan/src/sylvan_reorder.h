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

#ifndef SYLVAN_VAR_REORDER_H
#define SYLVAN_VAR_REORDER_H

#include "sylvan_varswap.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sifting_state
{
    BDDLABEL pos;
    size_t size;
    BDDLABEL best_pos;
    size_t best_size;
    BDDLABEL low;
    BDDLABEL high;
} sifting_state_t;

typedef int (*reorder_termination_cb)();

// opaque type
typedef struct reorder_config *reorder_config_t;

void sylvan_init_reorder(void);

void sylvan_quit_reorder(void);

reorder_config_t sylvan_get_reorder_config();

void sylvan_set_reorder_terminationcb(reorder_termination_cb callback);

/**
 * @brief Set threshold for the number of nodes per level to consider during the reordering.
 * @details If the number of nodes per level is less than the threshold, the level is skipped during the reordering.
 *         The default value is 32.
 * @param threshold The threshold for the number of nodes per level.
*/
void sylvan_set_reorder_threshold(uint32_t threshold);

/**
 * @brief Set the maximum growth coefficient.
 * @details The maximum growth coefficient is used to calculate the maximum growth of the number of nodes during the reordering.
 *        The default value is 1.2. If the number of nodes grows more than the maximum growth coefficient , sift up/down is terminated.
 * @param max_growth The maximum growth coefficient.
*/
void sylvan_set_reorder_maxgrowth(float max_growth);

/**
 * @brief Set the maximum number of swaps per sifting.
 * @details The default value is 10000.
 * @param max_swap The maximum number of swaps per sifting.
*/
void sylvan_set_reorder_maxswap(uint32_t max_swap);

/**
 * @brief Set the maximum number of vars swapped per sifting.
 * @details The default value is 2000.
 * @param max_var The maximum number of vars swapped per sifting.
*/
void sylvan_set_reorder_maxvar(uint32_t max_var);

/**
 * @brief Set the time limit for the reordering.
 * @details The default value is 50000 milliseconds.
 * @param time_limit The time limit for the reordering.
*/
void sylvan_set_reorder_timelimit(double time_limit);

TASK_DECL_1(varswap_t, sylvan_siftdown, sifting_state_t*);
/**
 * \brief Sift given variable up from its current level to the target level.
 *
 * \param var - variable to sift up
 * \param high - target position
 * \param curSize - pointer to current size of the bdd
 * \param bestSize - pointer to best size of the bdd (w.r.t. dynamic variable reordering)
 * \param bestPos - pointer to best position of the variable (w.r.t. dynamic variable reordering)
 *
 * \sideeffect order of variables is changed
 */
#define sylvan_siftdown(state) RUN(sylvan_siftdown, state)

TASK_DECL_1(varswap_t, sylvan_siftup, sifting_state_t*);
/**
 * \brief Sift given variable down from its current level to the target level.
 *
 * \param var - variable to sift down
 * \param low - target level
 * \param curSize - pointer to current size of the bdd
 * \param bestSize - pointer to best size of the bdd (w.r.t. dynamic variable reordering)
 * \param bestPos - pointer to best position of the variable (w.r.t. dynamic variable reordering)
 *
 * \sideeffect order of variables is changed
 */
#define sylvan_siftup(state) RUN(sylvan_siftup, state)

TASK_DECL_2(varswap_t, sylvan_siftpos, BDDLABEL, BDDLABEL);
/**
 * \brief Sift a variable to its best level.
 * \param var - variable to sift
 * \param targetPos - target position (w.r.t. dynamic variable reordering)
 */
#define sylvan_siftpos(pos, target) RUN(sylvan_siftpos, pos, target)

TASK_DECL_2(varswap_t, sylvan_reorder, BDDLABEL, BDDLABEL);
/**
      @brief Reduce the heap size in the entire forest.

      @details Implementation of Rudell's sifting algorithm.
        <ol>
        <li> Order all the variables according to the number of entries
        in each unique table.
        <li> Sift the variable up and down, remembering each time the
        total size of the bdd size.
        <li> Select the best permutation.
        <li> Repeat 2 and 3 for all variables in given range.
        </ol>

      @param low - the lowest position to sift
      @param high - the highest position to sift
*/
#define sylvan_reorder(low, high)  RUN(sylvan_reorder, low, high)

/**
      @brief Reduce the heap size in the entire forest.

      @details Implementation of Rudell's sifting algorithm.
        <ol>
        <li> Order all the variables according to the number of entries
        in each unique table.
        <li> Sift the variable up and down, remembering each time the
        total size of the bdd size.
        <li> Select the best permutation.
        <li> Repeat 2 and 3 for all variables in given range.
        </ol>
*/
#define sylvan_reorder_all()  sylvan_reorder(0, 0)

TASK_DECL_1(varswap_t, sylvan_reorder_perm, const uint32_t*);
/**
  @brief Reorder the variables in the BDDs according to the given permutation.

  @details The permutation is an array of BDD labels, where the i-th element is the label
  of the variable that should be moved to position i. The size
  of the array should be equal or greater to the number of variables
  currently in use.
 */
#define sylvan_reorder_perm(permutation)  RUN(sylvan_reorder_perm, permutation)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //SYLVAN_VAR_REORDER_H


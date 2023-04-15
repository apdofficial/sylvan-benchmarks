#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sylvan.h"
#include "test_assert.h"
#include "sylvan_int.h"
#include "sylvan_varswap.h"
#include "sylvan_reorder.h"
#include "sylvan_levels.h"
#include "common.h"

TASK_1(BDD, create_example_bdd, size_t, is_optimal)
{
//    BDD is from the paper:
//    Randal E. Bryant Graph-Based Algorithms for Boolean Function Manipulation,
//    IEEE Transactions on Computers, 1986 http://www.cs.cmu.edu/~bryant/pubdir/ieeetc86.pdf
    BDD v0 = sylvan_newlevel();
    BDD v1 = sylvan_newlevel();
    BDD v2 = sylvan_newlevel();
    BDD v3 = sylvan_newlevel();
    BDD v4 = sylvan_newlevel();
    BDD v5 = sylvan_newlevel();

    BDD bdd;
    if (is_optimal) {
        // optimal order 0, 1, 2, 3, 4, 5
        // minimum 8 nodes including 2 terminal nodes
        bdd = sylvan_or(sylvan_and(v0, v1), sylvan_or(sylvan_and(v2, v3), sylvan_and(v4, v5)));
    } else {
        // not optimal order 0, 3, 1, 4, 2, 5
        // minimum 16 nodes including 2 terminal nodes
        bdd = sylvan_or(sylvan_and(v0, v3), sylvan_or(sylvan_and(v1, v4), sylvan_and(v2, v5)));
    }
    return bdd;
}
#define create_example_bdd(is_optimal) RUN(create_example_bdd, is_optimal)

TASK_1(BDDMAP, create_example_map, size_t, is_optimal)
{
    BDDMAP map = sylvan_map_empty();
    BDD bdd = create_example_bdd(is_optimal);
    map = sylvan_map_add(map, 0, bdd);
    return map;
}
#define create_example_map(is_optimal) RUN(create_example_map, is_optimal)

TASK_0(int, test_varswap)
{
    // we need fixed variables for the test
    // therefore we reset the levels and create new one
    sylvan_gc();

    sylvan_resetlevels();
    sylvan_newlevels(10);

    /* test ithvar, switch 6 and 7 */
    BDD one = sylvan_ithlevel(6);
    BDD two = sylvan_ithlevel(7);

    test_assert(sylvan_level_to_var(6) == 6);
    test_assert(sylvan_level_to_var(7) == 7);
    test_assert(sylvan_var_to_level(6) == 6);
    test_assert(sylvan_var_to_level(7) == 7);
    test_assert(one == sylvan_ithvar(6));
    test_assert(two == sylvan_ithvar(7));
    test_assert(mtbdd_getvar(one) == 6);
    test_assert(mtbdd_getvar(two) == 7);

    test_assert(sylvan_varswap(6) == SYLVAN_VARSWAP_SUCCESS);

    test_assert(sylvan_level_to_var(7) == 6);
    test_assert(sylvan_level_to_var(6) == 7);
    test_assert(sylvan_var_to_level(7) == 6);
    test_assert(sylvan_var_to_level(6) == 7);
    test_assert(mtbdd_getvar(one) == 7);
    test_assert(mtbdd_getvar(two) == 6);
    test_assert(one == sylvan_ithvar(7));
    test_assert(two == sylvan_ithvar(6));

    return 0;
}

TASK_0(int, test_varswap_down)
{
    // we need fixed variables for the test
    // therefore we reset the levels and create new one
    sylvan_gc();
    sylvan_resetlevels();
    sylvan_newlevels(4);

    MTBDD zero = sylvan_ithlevel(0);
    MTBDD one = sylvan_ithlevel(1);
    MTBDD two = sylvan_ithlevel(2);
    MTBDD three = sylvan_ithlevel(3);

    /* swap down manually var 0 to level 3 */
    test_assert(sylvan_level_to_var(0) == 0);
    test_assert(sylvan_level_to_var(1) == 1);
    test_assert(sylvan_level_to_var(2) == 2);
    test_assert(sylvan_level_to_var(3) == 3);

    test_assert(sylvan_var_to_level(0) == 0);
    test_assert(sylvan_var_to_level(1) == 1);
    test_assert(sylvan_var_to_level(2) == 2);
    test_assert(sylvan_var_to_level(3) == 3);

    test_assert(zero == sylvan_ithvar(0));
    test_assert(one == sylvan_ithvar(1));
    test_assert(two == sylvan_ithvar(2));
    test_assert(three == sylvan_ithvar(3));

    test_assert(mtbdd_getvar(zero) == 0);
    test_assert(mtbdd_getvar(one) == 1);
    test_assert(mtbdd_getvar(two) == 2);
    test_assert(mtbdd_getvar(three) == 3);

    // 0, 1, 2, 3
    test_assert(sylvan_varswap(0) == SYLVAN_VARSWAP_SUCCESS);
    test_assert(sylvan_varswap(1) == SYLVAN_VARSWAP_SUCCESS);
    test_assert(sylvan_varswap(2) == SYLVAN_VARSWAP_SUCCESS);
    // 1, 2, 3, 0

    test_assert(sylvan_level_to_var(0) == 1);
    test_assert(sylvan_level_to_var(1) == 2);
    test_assert(sylvan_level_to_var(2) == 3);
    test_assert(sylvan_level_to_var(3) == 0);

    test_assert(sylvan_var_to_level(1) == 0);
    test_assert(sylvan_var_to_level(2) == 1);
    test_assert(sylvan_var_to_level(3) == 2);
    test_assert(sylvan_var_to_level(0) == 3);

    test_assert(zero == sylvan_ithvar(3));
    test_assert(one == sylvan_ithvar(0));
    test_assert(two == sylvan_ithvar(1));
    test_assert(three == sylvan_ithvar(2));

    test_assert(mtbdd_getvar(zero) == 3);
    test_assert(mtbdd_getvar(one) == 0);
    test_assert(mtbdd_getvar(two) == 1);
    test_assert(mtbdd_getvar(three) == 2);

    return 0;
}

TASK_0(int, test_varswap_up)
{
    // we need fixed variables for the test
    // therefore we reset the levels and create new one
    sylvan_gc();
    sylvan_resetlevels();
    sylvan_newlevels(4);

    MTBDD zero = sylvan_ithlevel(0);
    MTBDD one = sylvan_ithlevel(1);
    MTBDD two = sylvan_ithlevel(2);
    MTBDD three = sylvan_ithlevel(3);

    /* swap up manually var 3 to level 0 */
    test_assert(zero == sylvan_ithvar(0));
    test_assert(one == sylvan_ithvar(1));
    test_assert(two == sylvan_ithvar(2));
    test_assert(three == sylvan_ithvar(3));

    test_assert(mtbdd_getvar(zero) == 0);
    test_assert(mtbdd_getvar(one) == 1);
    test_assert(mtbdd_getvar(two) == 2);
    test_assert(mtbdd_getvar(three) == 3);

    // 0, 1, 2, 3
    test_assert(sylvan_varswap(2) == SYLVAN_VARSWAP_SUCCESS);
    test_assert(sylvan_varswap(1) == SYLVAN_VARSWAP_SUCCESS);
    test_assert(sylvan_varswap(0) == SYLVAN_VARSWAP_SUCCESS);
    // 3, 0, 1, 2

    test_assert(sylvan_level_to_var(0) == 3);
    test_assert(sylvan_level_to_var(1) == 0);
    test_assert(sylvan_level_to_var(2) == 1);
    test_assert(sylvan_level_to_var(3) == 2);

    test_assert(sylvan_var_to_level(3) == 0);
    test_assert(sylvan_var_to_level(0) == 1);
    test_assert(sylvan_var_to_level(1) == 2);
    test_assert(sylvan_var_to_level(2) == 3);

    test_assert(zero == sylvan_ithvar(1));
    test_assert(one == sylvan_ithvar(2));
    test_assert(two == sylvan_ithvar(3));
    test_assert(three == sylvan_ithvar(0));

    test_assert(mtbdd_getvar(zero) == 1);
    test_assert(mtbdd_getvar(one) == 2);
    test_assert(mtbdd_getvar(two) == 3);
    test_assert(mtbdd_getvar(three) == 0);

    return 0;
}

TASK_0(int, test_sift_down)
{
    // we need fixed variables for the test
    // therefore we reset the levels and create new one
    sylvan_gc();
    sylvan_resetlevels();
    sylvan_newlevels(4);

    MTBDD zero = sylvan_ithlevel(0);
    MTBDD one = sylvan_ithlevel(1);
    MTBDD two = sylvan_ithlevel(2);
    MTBDD three = sylvan_ithlevel(3);

    /* swap down manually var 0 to level 3 */
    test_assert(sylvan_level_to_var(0) == 0);
    test_assert(sylvan_level_to_var(1) == 1);
    test_assert(sylvan_level_to_var(2) == 2);
    test_assert(sylvan_level_to_var(3) == 3);

    test_assert(sylvan_var_to_level(0) == 0);
    test_assert(sylvan_var_to_level(1) == 1);
    test_assert(sylvan_var_to_level(2) == 2);
    test_assert(sylvan_var_to_level(3) == 3);

    test_assert(zero == sylvan_ithvar(0));
    test_assert(one == sylvan_ithvar(1));
    test_assert(two == sylvan_ithvar(2));
    test_assert(three == sylvan_ithvar(3));

    test_assert(mtbdd_getvar(zero) == 0);
    test_assert(mtbdd_getvar(one) == 1);
    test_assert(mtbdd_getvar(two) == 2);
    test_assert(mtbdd_getvar(three) == 3);

    sifting_state_t state;
    state.size = llmsset_count_marked(nodes);
    state.best_size = state.size;
    state.pos = 0;
    state.best_pos = 0;
    state.low = 0;
    state.high = 3;

    // 0, 1, 2, 3
    test_assert(sylvan_siftdown(&state) == SYLVAN_VARSWAP_SUCCESS);
    // 1, 2, 3, 0

    test_assert(sylvan_level_to_var(0) == 1);
    test_assert(sylvan_level_to_var(1) == 2);
    test_assert(sylvan_level_to_var(2) == 3);
    test_assert(sylvan_level_to_var(3) == 0);

    test_assert(sylvan_var_to_level(1) == 0);
    test_assert(sylvan_var_to_level(2) == 1);
    test_assert(sylvan_var_to_level(3) == 2);
    test_assert(sylvan_var_to_level(0) == 3);

    test_assert(zero == sylvan_ithvar(3));
    test_assert(one == sylvan_ithvar(0));
    test_assert(two == sylvan_ithvar(1));
    test_assert(three == sylvan_ithvar(2));

    test_assert(mtbdd_getvar(zero) == 3);
    test_assert(mtbdd_getvar(one) == 0);
    test_assert(mtbdd_getvar(two) == 1);
    test_assert(mtbdd_getvar(three) == 2);

    return 0;
}

TASK_0(int, test_sift_up)
{
    // we need fixed variables for the test
    // therefore we reset the levels and create new one
    sylvan_gc();
    sylvan_resetlevels();
    sylvan_newlevels(4);

    MTBDD zero = sylvan_ithlevel(0);
    MTBDD one = sylvan_ithlevel(1);
    MTBDD two = sylvan_ithlevel(2);
    MTBDD three = sylvan_ithlevel(3);

    /* swap up manually var 3 to level 0 */
    test_assert(zero == sylvan_ithvar(0));
    test_assert(one == sylvan_ithvar(1));
    test_assert(two == sylvan_ithvar(2));
    test_assert(three == sylvan_ithvar(3));

    test_assert(mtbdd_getvar(zero) == 0);
    test_assert(mtbdd_getvar(one) == 1);
    test_assert(mtbdd_getvar(two) == 2);
    test_assert(mtbdd_getvar(three) == 3);

    sifting_state_t state;
    state.size = llmsset_count_marked(nodes);
    state.best_size = state.size;
    state.pos = 3;
    state.best_pos = 0;
    state.low = 0;
    state.high = 3;

    // 0, 1, 2, 3
    test_assert(sylvan_siftup(&state) == SYLVAN_VARSWAP_SUCCESS);
    // 3, 0, 1, 2

    test_assert(sylvan_level_to_var(0) == 3);
    test_assert(sylvan_level_to_var(1) == 0);
    test_assert(sylvan_level_to_var(2) == 1);
    test_assert(sylvan_level_to_var(3) == 2);

    test_assert(sylvan_var_to_level(3) == 0);
    test_assert(sylvan_var_to_level(0) == 1);
    test_assert(sylvan_var_to_level(1) == 2);
    test_assert(sylvan_var_to_level(2) == 3);

    test_assert(zero == sylvan_ithvar(1));
    test_assert(one == sylvan_ithvar(2));
    test_assert(two == sylvan_ithvar(3));
    test_assert(three == sylvan_ithvar(0));

    test_assert(mtbdd_getvar(zero) == 1);
    test_assert(mtbdd_getvar(one) == 2);
    test_assert(mtbdd_getvar(two) == 3);
    test_assert(mtbdd_getvar(three) == 0);
    return 0;
}

TASK_0(int, test_sift_pos)
{
    // we need fixed variables for the test
    // therefore we reset the levels and create new one
    sylvan_gc();
    sylvan_resetlevels();
    sylvan_newlevels(4);

    MTBDD zero = sylvan_ithlevel(0);
    MTBDD one = sylvan_ithlevel(1);
    MTBDD two = sylvan_ithlevel(2);
    MTBDD three = sylvan_ithlevel(3);

    /* swap up manually var 3 to level 0 */
    test_assert(zero == sylvan_ithvar(0));
    test_assert(one == sylvan_ithvar(1));
    test_assert(two == sylvan_ithvar(2));
    test_assert(three == sylvan_ithvar(3));

    test_assert(mtbdd_getvar(zero) == 0);
    test_assert(mtbdd_getvar(one) == 1);
    test_assert(mtbdd_getvar(two) == 2);
    test_assert(mtbdd_getvar(three) == 3);

    // 0, 1, 2, 3
    test_assert(sylvan_siftpos(3, 0) == SYLVAN_VARSWAP_SUCCESS);
    // 3, 0, 1, 2

    test_assert(sylvan_level_to_var(0) == 3);
    test_assert(sylvan_level_to_var(1) == 0);
    test_assert(sylvan_level_to_var(2) == 1);
    test_assert(sylvan_level_to_var(3) == 2);

    test_assert(sylvan_var_to_level(3) == 0);
    test_assert(sylvan_var_to_level(0) == 1);
    test_assert(sylvan_var_to_level(1) == 2);
    test_assert(sylvan_var_to_level(2) == 3);

    test_assert(zero == sylvan_ithvar(1));
    test_assert(one == sylvan_ithvar(2));
    test_assert(two == sylvan_ithvar(3));
    test_assert(three == sylvan_ithvar(0));

    test_assert(mtbdd_getvar(zero) == 1);
    test_assert(mtbdd_getvar(one) == 2);
    test_assert(mtbdd_getvar(two) == 3);
    test_assert(mtbdd_getvar(three) == 0);

    // 3, 0, 1, 2
    test_assert(sylvan_siftpos(0, 3) == SYLVAN_VARSWAP_SUCCESS);
    // 0, 1, 2, 3

    test_assert(zero == sylvan_ithvar(0));
    test_assert(one == sylvan_ithvar(1));
    test_assert(two == sylvan_ithvar(2));
    test_assert(three == sylvan_ithvar(3));

    test_assert(mtbdd_getvar(zero) == 0);
    test_assert(mtbdd_getvar(one) == 1);
    test_assert(mtbdd_getvar(two) == 2);
    test_assert(mtbdd_getvar(three) == 3);

    return 0;
}

TASK_0(int, test_reorder_perm)
{
    // we need fixed variables for the test
    // therefore we reset the levels and create new one
    sylvan_gc();
    sylvan_resetlevels();
    sylvan_newlevels(4);

    MTBDD zero = sylvan_ithlevel(0);
    MTBDD one = sylvan_ithlevel(1);
    MTBDD two = sylvan_ithlevel(2);
    MTBDD three = sylvan_ithlevel(3);

    /* reorder the variables according to the variable permutation*/
    test_assert(zero == sylvan_ithvar(0));
    test_assert(one == sylvan_ithvar(1));
    test_assert(two == sylvan_ithvar(2));
    test_assert(three == sylvan_ithvar(3));

    test_assert(mtbdd_getvar(zero) == 0);
    test_assert(mtbdd_getvar(one) == 1);
    test_assert(mtbdd_getvar(two) == 2);
    test_assert(mtbdd_getvar(three) == 3);

    uint32_t perm[4] = {3, 0, 2, 1};

    test_assert(sylvan_reorder_perm(perm) == SYLVAN_VARSWAP_SUCCESS);

    test_assert(sylvan_level_to_var(0) == perm[0]);
    test_assert(sylvan_level_to_var(1) == perm[1]);
    test_assert(sylvan_level_to_var(2) == perm[2]);
    test_assert(sylvan_level_to_var(3) == perm[3]);

    test_assert(sylvan_var_to_level(perm[0]) == 0);
    test_assert(sylvan_var_to_level(perm[1]) == 1);
    test_assert(sylvan_var_to_level(perm[2]) == 2);
    test_assert(sylvan_var_to_level(perm[3]) == 3);

    test_assert(zero == sylvan_ithvar(1));
    test_assert(one == sylvan_ithvar(3));
    test_assert(two == sylvan_ithvar(2));
    test_assert(three == sylvan_ithvar(0));

    test_assert(mtbdd_getvar(zero) == 1);
    test_assert(mtbdd_getvar(one) == 3);
    test_assert(mtbdd_getvar(two) == 2);
    test_assert(mtbdd_getvar(three) == 0);

    return 0;
}

TASK_0(int, test_reorder)
{
    // we need fixed variables for the test
    // therefore we reset the levels and create new ones
    sylvan_gc();
    sylvan_resetlevels();

    BDD bdd = create_example_bdd(0);
    sylvan_protect(&bdd);

    size_t not_optimal_order_size = sylvan_nodecount(bdd);
    sylvan_reorder_all();
    size_t not_optimal_order_reordered_size = sylvan_nodecount(bdd);

    test_assert(not_optimal_order_reordered_size < not_optimal_order_size);

    uint32_t perm[6] = { 0, 1, 2, 3, 4, 5 };
    int identity = 1;
    // check if the new order is identity with the old order
    for (size_t i = 0; i < sylvan_levelscount(); i++) {
        if (sylvan_var_to_level(i) != perm[i]) {
            identity = 0;
            break;
        }
    }

//     if we gave it not optimal ordering then the new ordering should not be identity
    test_assert(identity == 0);

    sylvan_reorder_perm(perm);
    size_t not_optimal_size_again = sylvan_nodecount(bdd);
    test_assert(not_optimal_order_size == not_optimal_size_again);

    for (size_t i = 0; i < sylvan_levelscount(); i++) {
        test_assert(sylvan_var_to_level(i) == perm[i]);
    }

    sylvan_unprotect(&bdd);

    return 0;
}

TASK_0(int, test_map_reorder)
{
    BDDMAP map = create_example_map(0);
    sylvan_protect(&map);

    size_t size_before = sylvan_nodecount(map);
    sylvan_reorder_all();
    size_t size_after = sylvan_nodecount(map);

    test_assert(size_after < size_before);
    sylvan_unprotect(&map);

    return 0;
}

TASK_1(int, runtests, size_t, ntests)
{
    printf("test_varswap\n");
    for (size_t j=0;j<ntests;j++) if (RUN(test_varswap)) return 1;
    printf("test_varswap_down\n");
    for (size_t j=0;j<ntests;j++) if (RUN(test_varswap_down)) return 1;
    printf("test_varswap_up\n");
    for (size_t j=0;j<ntests;j++) if (RUN(test_varswap_up)) return 1;
    printf("test_sift_down\n");
    for (size_t j=0;j<ntests;j++) if (RUN(test_sift_down)) return 1;
    printf("test_sift_up\n");
    for (size_t j=0;j<ntests;j++) if (RUN(test_sift_up)) return 1;
    printf("test_sift_pos\n");
    for (size_t j=0;j<ntests;j++) if (RUN(test_sift_pos)) return 1;
    printf("test_reorder_perm\n");
    for (size_t j=0;j<ntests;j++) if (RUN(test_reorder_perm)) return 1;
    printf("test_reorder\n");
    for (size_t j=0;j<ntests;j++) if (RUN(test_reorder)) return 1;
    printf("test_map_reorder\n");
    for (size_t j=0;j<ntests;j++) if (RUN(test_map_reorder)) return 1;
    return 0;
}

int main()
{
    lace_start(4, 1000000); // 4 workers, use a 1,000,000 size task queue

    sylvan_set_sizes(4LL<<20, 1LL<<20, 1LL<<16, 1LL<<16);
    sylvan_init_package();
    sylvan_init_mtbdd();
    sylvan_init_reorder();
    sylvan_gc_enable();

    sylvan_set_reorder_threshold(1);
    sylvan_set_reorder_maxgrowth(1.2f);
    sylvan_set_reorder_timelimit(1 * 1000); // 1 second

    size_t ntests = 5;

    int res = RUN(runtests, ntests);

    sylvan_quit();
    lace_stop();

    return res;
}

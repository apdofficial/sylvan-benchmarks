#include "sylvan.h"
#include "sylvan_reorder.h"

void sylvan_start()
{
    lace_start(4, 1000000); // 4 workers, use a 1,000,000 size task queue

    sylvan_set_limits(1LL * 1LL << 30, 1, 128);
    sylvan_init_package();
    sylvan_init_mtbdd();
    sylvan_init_reorder();
    sylvan_gc_enable();

    sylvan_set_reorder_nodes_threshold(1);
    sylvan_set_reorder_maxgrowth(1.2f);
    sylvan_set_reorder_timelimit_sec(30); // 1 minute
}

void sylvan_exit()
{
    sylvan_quit();
    lace_stop();
}

//    BDD is from the paper:
//    Randal E. Bryant Graph-Based Algorithms for Boolean Function Manipulation,
//    IEEE Transactions on Computers, 1986 http://www.cs.cmu.edu/~bryant/pubdir/ieeetc86.pdf
#define sylvan_create_example_bdd(is_optimal) RUN(create_example_bdd, is_optimal)
TASK_1(BDD, create_example_bdd, size_t, is_optimal)
{
    // the variable indexing is relative to the current level
    BDD v0 = sylvan_newlevel();
    BDD v1 = sylvan_newlevel();
    BDD v2 = sylvan_newlevel();
    BDD v3 = sylvan_newlevel();
    BDD v4 = sylvan_newlevel();
    BDD v5 = sylvan_newlevel();

    if (is_optimal) {
        // optimal order 0, 1, 2, 3, 4, 5
        // minimum 8 nodes including 2 terminal nodes
        return sylvan_or(sylvan_and(v0, v1), sylvan_or(sylvan_and(v2, v3), sylvan_and(v4, v5)));
    } else {
        // not optimal order 0, 3, 1, 4, 2, 5
        // minimum 16 nodes including 2 terminal nodes
        return sylvan_or(sylvan_and(v0, v3), sylvan_or(sylvan_and(v1, v4), sylvan_and(v2, v5)));
    }
}
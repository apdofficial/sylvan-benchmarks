#include "sylvan.h"
#include "sylvan_int.h"
#include "sylvan_reorder.h"

void sylvan_start()
{
    lace_start(4, 1000000); // 4 workers, use a 1,000,000 size task queue

    sylvan_set_limits(1LL << 30, 1, 12);
    sylvan_init_package();
    sylvan_init_mtbdd();
    sylvan_init_reorder();
    sylvan_gc_enable();

    sylvan_set_reorder_nodes_threshold(2);
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
    BDD v0 = sylvan_ithvar(0);
    BDD v1 = sylvan_ithvar(1);
    BDD v2 = sylvan_ithvar(2);
    BDD v3 = sylvan_ithvar(3);
    BDD v4 = sylvan_ithvar(4);
    BDD v5 = sylvan_ithvar(5);

    if (is_optimal) {
        // optimal order 0, 1, 2, 3, 4, 5
        // minimum 8 nodes including 2 terminal nodes
        BDD a = sylvan_and(v0, v1);
        BDD b = sylvan_and(v2, v3);
        BDD c = sylvan_and(v4, v5);
        return sylvan_or(a, sylvan_or(b, c));
    } else {
        // not optimal order 0, 3, 1, 4, 2, 5
        // minimum 16 nodes including 2 terminal nodes
        BDD a = sylvan_and(v0, v3);
        BDD b = sylvan_and(v1, v4);
        BDD c = sylvan_and(v2, v5);
        return sylvan_or(a, sylvan_or(b, c));
    }
}
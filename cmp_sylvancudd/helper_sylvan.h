#include "sylvan.h"
#include "sylvan_int.h"
#include "sylvan_reorder.h"

void sylvan_start()
{
    lace_start(1, 1000000); // 4 workers, use a 1,000,000 size task queue

    sylvan_set_limits(1LL << 30, 1, 12);
    sylvan_init_package();
    sylvan_init_mtbdd();
    sylvan_init_reorder();
    sylvan_gc_enable();

    sylvan_set_reorder_nodes_threshold(1);
    sylvan_set_reorder_maxgrowth(1.2f);
    sylvan_set_reorder_timelimit_sec(6000); // 1 minute
}

void sylvan_exit()
{
    sylvan_quit();
    lace_stop();
}

//    BDD is from the paper:
//    Randal E. Bryant Graph-Based Algorithms for Boolean Function Manipulation,
//    IEEE Transactions on Computers, 1986 http://www.cs.cmu.edu/~bryant/pubdir/ieeetc86.pdf
#define sylvan_create_example_bdd(is_optimal, offset_var) RUN(create_example_bdd, is_optimal, offset_var)
TASK_2(BDD, create_example_bdd, size_t, is_optimal, int, offset_var)
{
    // the variable indexing is relative to the current level
    BDD v0 = sylvan_ithvar(offset_var);
    sylvan_ref(v0);
    BDD v1 = sylvan_ithvar(offset_var + 1);
    sylvan_ref(v1);
    BDD v2 = sylvan_ithvar(offset_var + 2);
    sylvan_ref(v2);
    BDD v3 = sylvan_ithvar(offset_var + 3);
    sylvan_ref(v3);
    BDD v4 = sylvan_ithvar(offset_var + 4);
    sylvan_ref(v4);
    BDD v5 = sylvan_ithvar(offset_var + 5);
    sylvan_ref(v5);

    if (is_optimal) {
        // optimal order 0, 1, 2, 3, 4, 5
        // minimum 8 nodes including 2 terminal nodes
        BDD a = sylvan_and(v0, v1);
        sylvan_ref(a);
        BDD b = sylvan_and(v2, v3);
        sylvan_ref(b);
        BDD c = sylvan_and(v4, v5);
        sylvan_ref(c);
        BDD final = sylvan_or(a, sylvan_or(b, c));
        sylvan_ref(final);
        return final;
    } else {
        // not optimal order 0, 3, 1, 4, 2, 5
        // minimum 16 nodes including 2 terminal nodes
        BDD a = sylvan_and(v0, v3);
        sylvan_ref(a);
        BDD b = sylvan_and(v1, v4);
        sylvan_ref(b);
        BDD c = sylvan_and(v2, v5);
        sylvan_ref(c);
        BDD final = sylvan_or(a, sylvan_or(b, c));
        return final;
    }
}
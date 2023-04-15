/**
 * Just a small test file to ensure that Sylvan can compile in C++
 */

#include <iostream>
#include <assert.h>
#include <sylvan.h>
#include <sylvan_obj.hpp>

#include "test_assert.h"

using namespace sylvan;

Bdd create_example_bdd(size_t is_optimal)
{
//    BDD is from the paper:
//    Randal E. Bryant Graph-Based Algorithms for Boolean Function Manipulation,
//    IEEE Transactions on Computers, 1986 http://www.cs.cmu.edu/~bryant/pubdir/ieeetc86.pdf
    Bdd v0 = Bdd::newLevel();
    Bdd v1 = Bdd::newLevel();
    Bdd v2 = Bdd::newLevel();
    Bdd v3 = Bdd::newLevel();
    Bdd v4 = Bdd::newLevel();
    Bdd v5 = Bdd::newLevel();

    if (is_optimal) {
        // optimal order 0, 1, 2, 3, 4, 5
        // minimum 8 nodes including 2 terminal nodes
        return (v0 + v1) * (v2 + v3) * (v4 + v5);
    } else {
        // not optimal order 0, 3, 1, 4, 2, 5
        // minimum 16 nodes including 2 terminal nodes
        return (v0 + v3) * (v1 + v4) * (v2 + v5);
    }
}

void test6()
{
    BddMap m1;
    BddMap m2(m1);  // this triggers an assertion
}

TASK_0(int, test_basics)
{
    test6();

    Bdd one = Bdd::bddOne();
    Bdd zero = Bdd::bddZero();

    test_assert(one != zero);
    test_assert(one == !zero);

    Bdd v1 = Bdd::bddVar(1);
    Bdd v2 = Bdd::bddVar(2);

    Bdd t = v1 + v2;
    Bdd u = v1;
    u += v2;

    test_assert(t == u);

    BddMap map;
    map.put(2, t);

    test_assert(v2.Compose(map) == (v1 + v2));
    test_assert((t * v2) == v2);

    return 0;
}

TASK_0(int, test_rordering)
{
    size_t is_optimal = 0;
    Bdd bdd = create_example_bdd(is_optimal);

    for (int i = 0; i < 6; ++i) {
        test_assert(Bdd::bddLevel(i) == Bdd::bddVar(i));
    }

    size_t not_optimal_order_size = bdd.NodeCount();
    Sylvan::reorderAll();
    size_t not_optimal_order_reordered_size = bdd.NodeCount();

    std::vector<uint32_t> perm = { 0, 1, 2, 3, 4, 5 };
    int identity = 1;
    // check if the new order is identity with the old order
    for (size_t i = 0; i < Sylvan::getLevelsCount(); i++) {
        if (Bdd::bddVar(perm.at(i)) != Bdd::bddLevel(i)) {
            identity = 0;
            break;
        }
    }

    // if we gave it not optimal ordering then the new ordering should not be identity
    test_assert(identity == 0);

    test_assert(not_optimal_order_reordered_size < not_optimal_order_size);

    // restore the original order
    Sylvan::reorderPerm(perm);
    size_t not_optimal_size_again = bdd.NodeCount();
    test_assert(not_optimal_order_size == not_optimal_size_again);

    for (int i = 0; i < 6; ++i) {
        test_assert(Bdd::bddLevel(i) == Bdd::bddVar(i));
    }

    return 0;
}

TASK_0(int, runtests)
{
    printf("test_rordering\n");
    if (RUN(test_rordering)) return 1;
    printf("test_basics\n");
    if (RUN(test_basics)) return 1;
    return 0;
}

int main()
{
    // Standard Lace initialization with 4 workers
    lace_start(4, 0);

    // Simple Sylvan initialization, also initialize BDD support
    Sylvan::initPackage(1LL<<16, 1LL<<16, 1LL<<16, 1LL<<16);
    Sylvan::initBdd();
    Sylvan::initReorder();

    Sylvan::setReorderMaxGrowth(1.2f);
    Sylvan::setReorderThreshold(1);
    Sylvan::setReorderTimeLimit(1 * 60 * 1000); // 1 minute
    Sylvan::setReorderMaxSwap(1000);
    Sylvan::setReorderMaxVar(500);

    int res = RUN(runtests);

    Sylvan::quitPackage();
    lace_stop();

    return res;
}

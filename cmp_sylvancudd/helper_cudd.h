#include "cudd.h"
#include "cuddInt.h"

//    BDD is from the paper:
//    Randal E. Bryant Graph-Based Algorithms for Boolean Function Manipulation,
//    IEEE Transactions on Computers, 1986 http://www.cs.cmu.edu/~bryant/pubdir/ieeetc86.pdf
DdNode *cudd_create_example_bdd(DdManager *db, int is_optimal)
{
    DdNode *v0 = Cudd_bddIthVar(db, 0);
    DdNode *v1 = Cudd_bddIthVar(db, 1);
    DdNode *v2 = Cudd_bddIthVar(db, 2);
    DdNode *v3 = Cudd_bddIthVar(db, 3);
    DdNode *v4 = Cudd_bddIthVar(db, 4);
    DdNode *v5 = Cudd_bddIthVar(db, 5);

    if (is_optimal) {
        // optimal order 0, 1, 2, 3, 4, 5
        // minimum 8 nodes including 2 terminal nodes
        return Cudd_bddOr(db, Cudd_bddAnd(db, v0, v1),
                          Cudd_bddOr(db, Cudd_bddAnd(db, v2, v3), Cudd_bddAnd(db, v4, v5)));
    } else {
        // not optimal order 0, 3, 1, 4, 2, 5
        // minimum 16 nodes including 2 terminal nodes
        return Cudd_bddOr(db, Cudd_bddAnd(db, v0, v1),
                          Cudd_bddOr(db, Cudd_bddAnd(db, v2, v3), Cudd_bddAnd(db, v4, v5)));
    }
}

DdManager *cudd_start()
{
    DdManager *db = Cudd_Init(6, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    return db;
}

void cudd_exit(DdManager *db)
{
    Cudd_Quit(db);
}


#include "cudd.h"
#include "cuddInt.h"

//    BDD is from the paper:
//    Randal E. Bryant Graph-Based Algorithms for Boolean Function Manipulation,
//    IEEE Transactions on Computers, 1986 http://www.cs.cmu.edu/~bryant/pubdir/ieeetc86.pdf
DdNode *cudd_create_example_bdd(DdManager *db, int is_optimal)
{
    DdNode *v0 = Cudd_bddIthVar(db, 0);
    Cudd_Ref(v0);
    DdNode *v1 = Cudd_bddIthVar(db, 1);
    Cudd_Ref(v1);
    DdNode *v2 = Cudd_bddIthVar(db, 2);
    Cudd_Ref(v2);
    DdNode *v3 = Cudd_bddIthVar(db, 3);
    Cudd_Ref(v3);
    DdNode *v4 = Cudd_bddIthVar(db, 4);
    Cudd_Ref(v4);
    DdNode *v5 = Cudd_bddIthVar(db, 5);
    Cudd_Ref(v5);

    if (is_optimal) {
        // optimal order 0, 1, 2, 3, 4, 5
        // minimum 8 nodes including 2 terminal nodes
        DdNode *a = Cudd_bddAnd(db, v0, v1);
        Cudd_Ref(a);
        DdNode *b = Cudd_bddAnd(db, v2, v3);
        Cudd_Ref(b);
        DdNode *c = Cudd_bddAnd(db, v4, v5);
        Cudd_Ref(c);
        DdNode *final = Cudd_bddOr(db, a, Cudd_bddOr(db, b, c));
        Cudd_Ref(final);
        return final;
    } else {
        // not optimal order 0, 3, 1, 4, 2, 5
        // minimum 16 nodes including 2 terminal nodes
        DdNode *a = Cudd_bddAnd(db, v0, v3);
        Cudd_Ref(a);
        DdNode *b = Cudd_bddAnd(db, v1, v4);
        Cudd_Ref(b);
        DdNode *c = Cudd_bddAnd(db, v2, v5);
        Cudd_Ref(c);
        DdNode *final = Cudd_bddOr(db, a, Cudd_bddOr(db, b, c));
        Cudd_Ref(final);
        return final;
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


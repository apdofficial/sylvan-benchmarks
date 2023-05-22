#include "helper_sylvan.h"
#include "helper_cudd.h"

int main(int argc, char **argv)
{
    DdManager *db = cudd_start();
//    sylvan_start();

    int is_optimal = 0;
//    BDD bdd_sylvan = sylvan_create_example_bdd(is_optimal);
    DdNode *bdd_cudd = cudd_create_example_bdd(db, is_optimal);
    Cudd_ReduceHeap(db, CUDD_REORDER_SIFT, 0);
    cudd_exit(db);
//    sylvan_exit();
}


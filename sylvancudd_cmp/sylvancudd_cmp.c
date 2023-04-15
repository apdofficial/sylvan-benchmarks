#include "helper_cudd.h"
#include "helper_sylvan.h"

int main(int argc, char **argv)
{
    DdManager *db = cudd_start();
    sylvan_start();

    int is_optimal = 1;
    BDD sylvan_bdd = sylvan_create_example_bdd(is_optimal);
    DdNode *bdd_cudd = cudd_create_example_bdd(db, is_optimal);

    cudd_exit();
    sylvan_exit();
}


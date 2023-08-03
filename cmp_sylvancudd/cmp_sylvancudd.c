#include "helper_sylvan.h"
#include "helper_cudd.h"

int run_cudd(int is_optimal);

int run_sylvan(int is_optimal);

int main(int argc, char **argv)
{
    int is_optimal = 1;
    run_cudd(is_optimal);
    run_sylvan(is_optimal);
}

int run_cudd(int is_optimal)
{
    DdManager *db = cudd_start();
    Cudd_EnableReorderingReporting(db);

    DdNode *bdd = cudd_create_example_bdd(db, is_optimal);
    Cudd_Ref(bdd);
    cuddGarbageCollect(db, 1);

    if (is_optimal){
        FILE *file = fopen("/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmp_sylvancudd/example_bdd/cudd_bdd_optimal.dot","w");
        Cudd_DumpDot(db, 1, &bdd, NULL, NULL, file);
    } else {
        FILE *file = fopen("/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmp_sylvancudd/example_bdd/cudd_bdd_not_optimal.dot","w");
        Cudd_DumpDot(db, 1, &bdd, NULL, NULL, file);
    }

    Cudd_ReduceHeap(db, CUDD_REORDER_SIFT, 0);

    if (is_optimal){
        FILE *file = fopen("/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmp_sylvancudd/example_bdd/cudd_bdd_optimal_reordered.dot","w");
        Cudd_DumpDot(db, 1, &bdd, NULL, NULL, file);
    } else {
        FILE *file = fopen("/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmp_sylvancudd/example_bdd/cudd_bdd_not_optimal_reordered.dot","w");
        Cudd_DumpDot(db, 1, &bdd, NULL, NULL, file);
    }

    cudd_exit(db);

    return 0;
}

int run_sylvan(int is_optimal)
{
    sylvan_start();

    BDD bdd = sylvan_create_example_bdd(is_optimal);
    sylvan_ref(bdd);
    sylvan_gc();

    if (is_optimal){
        FILE *file = fopen("/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmp_sylvancudd/example_bdd/sylvan_bdd_optimal.dot","w");
        sylvan_fprintdot(file, bdd);
    } else {
        FILE *file = fopen("/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmp_sylvancudd/example_bdd/sylvan_bdd_not_optimal.dot","w");
        sylvan_fprintdot(file, bdd);
    }

    sylvan_reduce_heap(SYLVAN_REORDER_BOUNDED_SIFT);

    if (is_optimal){
        FILE *file = fopen("/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmp_sylvancudd/example_bdd/sylvan_bdd_optimal_reordered.dot","w");
        sylvan_fprintdot(file, bdd);
    } else {
        FILE *file = fopen("/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmp_sylvancudd/example_bdd/sylvan_bdd_not_optimal_reordered.dot","w");
        sylvan_fprintdot(file, bdd);
    }

    sylvan_exit();
    return 0;
}


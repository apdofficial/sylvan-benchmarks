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

    int i, j;
    DdNode *f;
    DdNode *sentinel = &(db->sentinel);
    DdNodePtr *nodelist;
    unsigned int slots;

    for (i = 0; i < db->size; i++) {
        nodelist = db->subtables[i].nodelist;
        slots = db->subtables[i].slots;
        for (j = 0; j < slots; j++) {
            f = nodelist[j];
            while (f != sentinel) {
                printf("%d (%d), ", f->index, f->ref);
                f = f->next;
            }
        }
    }
    printf("\n");

    printf("table node count: %zu\n", Cudd_ReadNodeCount(db));
    printf("bdd node count: %d\n", Cudd_DagSize(bdd));


//    Cudd_ReduceHeap(db, CUDD_REORDER_SIFT, 0);


    cudd_exit(db);
}

int run_sylvan(int is_optimal)
{
    sylvan_start();

    BDD bdd = sylvan_create_example_bdd(is_optimal);
    sylvan_protect(&bdd);
    sylvan_gc();

    sylvan_pre_reorder();
    interact_var_ref_init(levels);

    // let's sort them based on the levels in quick ugly way (easier to compare with CUDD)
    for (int level = 0; level < sylvan_levelscount(); ++level){
        size_t index = llmsset_first();
        while (index != llmsset_nindex) {
            mtbddnode_t node = MTBDD_GETNODE(index);
            BDDVAR var = mtbddnode_getvariable(node);
            if (var == level) {
                counter_t int_ref_count = levels_node_ref_count_load(levels, index);
                printf("%d (%d), ", var, int_ref_count);
            }
            index = llmsset_next(index);
        }
    }
    printf("\n");


    printf("table node count: %zu\n", llmsset_count_marked(nodes)-2);
    printf("bdd node count: %zu\n", sylvan_nodecount(bdd));


//    sylvan_reduce_heap(SYLVAN_REORDER_BOUNDED_SIFT);

    sylvan_exit();
}


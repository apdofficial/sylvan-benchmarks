#include <sylvan.h>

BDD sylvan_ite_stub(BDD a, BDD b, BDD c){
    return sylvan_ite(a, b, c);
}

BDD sylvan_and_stub(BDD a, BDD b){
    return sylvan_and(a, b);
}

BDD sylvan_xor_stub(BDD a, BDD b){
    return sylvan_xor(a, b);
}

BDD sylvan_exists_stub(BDD a, BDD variables){
    return sylvan_exists(a, variables);
}

BDD sylvan_and_exists_stub(BDD a, BDD b, BDD variables){
    return sylvan_and_exists(a, b, variables);
}

BDD sylvan_compose_stub(BDD f, BDDMAP m){
    return sylvan_compose(f, m);
}

void sylvan_gc_stub(){
    sylvan_gc();
}

void sylvan_reduce_heap_bounded(){
    sylvan_reduce_heap(SYLVAN_REORDER_BOUNDED_SIFT);
}


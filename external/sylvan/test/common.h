#include <stdint.h>
#include <stdlib.h>

#include "sylvan.h"
#include "sylvan_int.h"

__thread uint64_t seed = 1;

void print_level_to_var(){
    printf("level_to_var: ");
    for (size_t i = 0; i < mtbdd_levelscount(); ++i){
        printf("%zu ", (size_t)mtbdd_level_to_var(i));
    }
    printf("\n");
}

void print_var_to_level(){
    printf("var_to_level: ");
    for (size_t i = 0; i < mtbdd_levelscount(); ++i){
        printf("%zu ", (size_t)mtbdd_var_to_level(i));
    }
    printf("\n");
}

int are_equal(BDD a, BDD b)
{
    if (a == b) return 1;

    if (a == sylvan_invalid) {
        fprintf(stderr, "a is invalid!\n");
        return 0;
    }

    if (b == sylvan_invalid) {
        fprintf(stderr, "b is invalid!\n");
        return 0;
    }

    fprintf(stderr, "a and b are not equal!\n");

    sylvan_fprint(stderr, a);fprintf(stderr, "\n");
    sylvan_fprint(stderr, b);fprintf(stderr, "\n");

    return 0;
}

uint64_t
xorshift_rand(void)
{
    uint64_t x = seed;
    if (seed == 0) seed = rand();
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    seed = x;
    return x * 2685821657736338717LL;
}

double
uniform_deviate(uint64_t seed)
{
    return seed * (1.0 / ((double)(UINT64_MAX) + 1.0));
}

int
rng(int low, int high)
{
    return low + uniform_deviate(xorshift_rand()) * (high-low);
}

/**
 * Make a random BDD
 */
BDD
make_random(int i, int j)
{
    if (i == j) return rng(0, 2) ? sylvan_true : sylvan_false;

    BDD yes = make_random(i+1, j);
    BDD no = make_random(i+1, j);
    BDD result = sylvan_invalid;

    switch(rng(0, 4)) {
    case 0:
        result = no;
        sylvan_deref(yes);
        break;
    case 1:
        result = yes;
        sylvan_deref(no);
        break;
    case 2:
        result = sylvan_ref(sylvan_makenode(i, yes, no));
        sylvan_deref(no);
        sylvan_deref(yes);
        break;
    case 3:
    default:
        result = sylvan_ref(sylvan_makenode(i, no, yes));
        sylvan_deref(no);
        sylvan_deref(yes);
        break;
    }

    return result;
}
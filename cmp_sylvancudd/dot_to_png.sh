#dot -Tpng example_bdd/cudd_bdd_optimal.dot > example_bdd/cudd_bdd_optimal.png && dot -Tpng example_bdd/sylvan_bdd_optimal.dot > example_bdd/sylvan_bdd_optimal.png
#dot -Tpng example_bdd/cudd_bdd_not_optimal.dot > example_bdd/cudd_bdd_not_optimal.png && dot -Tpng example_bdd/sylvan_bdd_not_optimal.dot > example_bdd/sylvan_bdd_not_optimal.png

dot -Tpng cudd_bdd.dot > cudd_bdd.png && dot -Tpng sylvan_bdd.dot > sylvan_bdd.png

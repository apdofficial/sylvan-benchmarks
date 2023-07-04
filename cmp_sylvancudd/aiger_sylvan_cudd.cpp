#include <getopt.h>

#include <sys/time.h>
#include <boost/graph/sloan_ordering.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <string>
#include <fcntl.h>
#include <span>

#include <sylvan_int.h>
#include <cuddInt.h>
#include "aag.h"


using namespace sylvan;

typedef struct safety_game
{
    MTBDD  *s_gates;            // Sylvan and gates
    DdNode **c_gates;            // CUDD and gates
    MTBDD   c_inputs;           // controllable inputs
    MTBDD   u_inputs;           // uncontrollable inputs
    int    *level_to_order;     // mapping from variable level to static variable order
} safety_game_t;

double t_start;
#define INFO(s, ...) fprintf(stdout, "\r[% 8.2f] " s, wctime()-t_start, ##__VA_ARGS__)
#define Abort(s, ...) { fprintf(stderr, "\r[% 8.2f] " s, wctime()-t_start, ##__VA_ARGS__); exit(-1); }


/* Configuration */
static int workers = 1;
static int verbose = 0;
static char *filename = nullptr; // filename of the aag file
static int static_reorder = 0;
static int dynamic_reorder = 0;
static int sloan_w1 = 1;
static int sloan_w2 = 8;

//static FILE *log_file = nullptr;

/* Global variables */
static aag_file_t aag{
        .header = {
                .m = 0,
                .i = 0,
                .l = 0,
                .o = 0,
                .a = 0,
                .b = 0,
                .c = 0,
                .j = 0,
                .f = 0
        },
        .inputs = nullptr,
        .outputs = nullptr,
        .latches = nullptr,
        .l_next = nullptr,
        .lookup = nullptr,
        .gatelhs = nullptr,
        .gatelft = nullptr,
        .gatergt = nullptr
};
static aag_buffer_t aag_buffer{
        .content = NULL,
        .size = 0,
        .pos = 0,
        .file_descriptor = -1,
        .filestat = {}
};
static safety_game_t game{
        .s_gates = nullptr,
        .c_inputs = sylvan_set_empty(),
        .u_inputs = sylvan_set_empty(),
        .level_to_order = nullptr
};

/* Obtain current wallclock time */
static double
wctime()
{
    struct timeval tv{};
    gettimeofday(&tv, nullptr);
    return (tv.tv_sec + 1E-6 * tv.tv_usec);
}

static void
print_usage()
{
    printf("Usage: aigsynt [-w <workers>] [-d --dynamic-reordering] [-s --static-reordering]\n");
    printf("               [-v --verbose] [--help] [--usage] <model> [<output-bdd>]\n");
}

static void
print_help()
{
    printf("Usage: aigsynt [OPTION...] <model> [<output-bdd>]\n\n");
    printf("                             Strategy for reachability (default=par)\n");
    printf("  -d,                        Dynamic variable ordering\n");
    printf("  -w, --workers=<workers>    Number of workers (default=0: autodetect)\n");
    printf("  -v,                        Dynamic variable ordering\n");
    printf("  -s,                        Reorder with Sloan\n");
    printf("  -h, --help                 Give this help list\n");
    printf("      --usage                Give a short usage message\n");
}

static void
parse_args(int argc, char **argv)
{
    static const option longopts[] = {
            {"workers",            required_argument, (int *) 'w', 1},
            {"dynamic-reordering", no_argument,       nullptr,     'd'},
            {"static-reordering",  no_argument,       nullptr,     's'},
            {"verbose",            no_argument,       nullptr,     'v'},
            {"help",               no_argument,       nullptr,     'h'},
            {"usage",              no_argument,       nullptr,     99},
            {nullptr,              no_argument,       nullptr,     0},
    };
    int key = 0;
    int long_index = 0;
    while ((key = getopt_long(argc, argv, "w:s:h", longopts, &long_index)) != -1) {
        switch (key) {
            case 'w':
                workers = atoi(optarg);
                break;
            case 's':
                static_reorder = 1;
                break;
            case 'd':
                dynamic_reorder = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 99:
                print_usage();
                exit(0);
            case 'h':
                print_help();
                exit(0);
        }
    }
    if (optind >= argc) {
        print_usage();
        exit(0);
    }
    filename = argv[optind];
}

VOID_TASK_0(gc_start)
{
    printf("\r[% 8.2f] GC: from (%zu / %zu) to ... ", wctime()-t_start, llmsset_count_marked(nodes), llmsset_get_size(nodes));
}

VOID_TASK_0(gc_end)
{
    printf("(%zu / %zu)\n", llmsset_count_marked(nodes), llmsset_get_size(nodes));
}

VOID_TASK_0(reordering_start)
{
    printf("\r[% 8.2f] RE: from %zu to ... ", wctime()-t_start, llmsset_count_marked(nodes));
}

VOID_TASK_0(reordering_end)
{
    printf("%zu nodes in %f\n", llmsset_count_marked(nodes), wctime() - reorder_db->config.t_start_sifting);
}

#define make_gate(gate, db) CALL(make_gate, gate, db)
VOID_TASK_2(make_gate, int, gate, DdManager*, db)
{
    if (game.s_gates[gate] != sylvan_invalid) return;
    int lft = (int) aag.gatelft[gate] / 2;
    int rgt = (int) aag.gatergt[gate] / 2;

    DdNode *c_l, *c_r;
    MTBDD l, r;

    if (lft == 0) {
        l = sylvan_false;
        c_l = CUDD_FALSE;
    } else if (aag.lookup[lft] != -1) {
        make_gate(aag.lookup[lft], db);
        l = game.s_gates[aag.lookup[lft]];
        c_l = game.c_gates[aag.lookup[lft]];
    } else {
        l = sylvan_ithvar(game.level_to_order[lft]); // always use even variables (prime is odd)
        c_l = Cudd_bddIthVar(db, game.level_to_order[lft]);
    }
    if (rgt == 0) {
        r = sylvan_false;
        c_r = CUDD_FALSE;
    } else if (aag.lookup[rgt] != -1) {
        make_gate(aag.lookup[rgt], db);
        r = game.s_gates[aag.lookup[rgt]];
        c_r = game.c_gates[aag.lookup[rgt]];
    } else {
        r = sylvan_ithvar(game.level_to_order[rgt]); // always use even variables (prime is odd)
        c_r = Cudd_bddIthVar(db, game.level_to_order[rgt]);
    }
    if (aag.gatelft[gate] & 1) {
        l = sylvan_not(l);
        c_l = Cudd_Not(c_l);
    }
    if (aag.gatergt[gate] & 1) {
        r = sylvan_not(r);
        c_r = Cudd_Not(c_r);
    }

    game.s_gates[gate] = sylvan_and(l, r);
    mtbdd_protect(&game.s_gates[gate]);

    game.c_gates[gate] = Cudd_bddAnd(db, c_l, c_r);
    Cudd_Ref(game.c_gates[gate]);
}

#define solve_game(db) RUN(solve_game, db)
TASK_1(int, solve_game, DdManager*, db)
{
    game.level_to_order = new int[aag.header.m + 1];
    for (int i = 0; i <= (int) aag.header.m; i++) game.level_to_order[i] = i;

    game.s_gates = new MTBDD[aag.header.a];
    for (uint64_t a = 0; a < aag.header.a; a++) game.s_gates[a] = sylvan_invalid;

    game.c_gates = new DdNode*[aag.header.a];
    for (uint64_t a = 0; a < aag.header.a; a++) game.c_gates[a] = nullptr;


    INFO("Making the gate BDDs...\n");

    for (uint64_t gate = 0; gate < aag.header.a; gate++) {
        make_gate(gate, db);
    }

    INFO("Sylvan gates have size %zu\n", mtbdd_nodecount_more(game.s_gates, aag.header.a));
    INFO("Cudd   gates have size %zu\n", Cudd_ReadNodeCount(db));

    sylvan_reduce_heap(SYLVAN_REORDER_BOUNDED_SIFT);
    Cudd_ReduceHeap(db, CUDD_REORDER_SIFT, 0);

    INFO("Sylvan gates have size %zu\n", mtbdd_nodecount_more(game.s_gates, aag.header.a));
    INFO("Cudd   gates have size %zu\n", Cudd_ReadNodeCount(db));

    exit(1);

    game.c_inputs = sylvan_set_empty();
    game.u_inputs = sylvan_set_empty();
    mtbdd_protect(&game.c_inputs);
    mtbdd_protect(&game.u_inputs);

    // Now read the [[optional]] labels to find controllable vars
    while (true) {
        int c = aag_buffer_peek(&aag_buffer);
        if (c != 'l' and c != 'i' and c != 'o') break;
        aag_buffer_skip(&aag_buffer);
        int pos = (int) aag_buffer_read_uint(&aag_buffer);
        aag_buffer_read_token(" ", &aag_buffer);
        std::string s;
        aag_buffer_read_string(s, &aag_buffer);
        aag_buffer_read_wsnl(&aag_buffer);
        if (c == 'i') {
            if (strncmp(s.c_str(), "controllable_", 13) == 0) {
                game.c_inputs = sylvan_set_add(game.c_inputs, game.level_to_order[aag.inputs[pos] / 2]);
            } else {
                game.u_inputs = sylvan_set_add(game.u_inputs, game.level_to_order[aag.inputs[pos] / 2]);
            }
        }
    }
    INFO("There are %zu controllable and %zu uncontrollable inputs.\n", sylvan_set_count(game.c_inputs), sylvan_set_count(game.u_inputs));

#if 0
    sylvan_print(Xc);
    printf("\n");
    sylvan_print(Xu);
    printf("\n");
    for (uint64_t g=0; g<A; g++) {
        INFO("gate %d has size %zu\n", (int)g, sylvan_nodecount(gates[g]));
    }

    for (uint64_t g=0; g<A; g++) {
        MTBDD supp = sylvan_support(gates[g]);
        while (supp != sylvan_set_empty()) {
            printf("%d ", sylvan_set_first(supp));
            supp = sylvan_set_next(supp);
        }
        printf("\n");
    }

    MTBDD lnext[L];
    for (uint64_t l=0; l<L; l++) {
        MTBDD nxt;
        if (lookup[l_next[l]/2] == -1) {
            nxt = sylvan_ithlevel(l_next[l]&1);
        } else {
            nxt = gates[lookup[l_next[l]]];
        }
        if (l_next[l]&1) nxt = sylvan_not(nxt);
        lnext[l] = sylvan_equiv(sylvan_ithvar(latches[l]+1), nxt);
    }
    INFO("done making latches\n");

    MTBDD Lvars = sylvan_set_empty();
    mtbdd_protect(&Lvars);

    for (uint64_t l = 0; l < aag.header.l; l++) {
        Lvars = sylvan_set_add(Lvars, game.level_to_order[aag.latches[l] / 2]);
    }

    MTBDD LtoPrime = sylvan_map_empty();
    for (uint64_t l=0; l<L; l++) {
        LtoPrime = sylvan_map_add(LtoPrime, latches[l], latches[l]+1);
    }
#endif

    // Actually just make the compose vector
    MTBDD CV = sylvan_map_empty();
    mtbdd_protect(&CV);

    for (uint64_t l = 0; l < aag.header.l; l++) {
        MTBDD nxt;
        if (aag.lookup[aag.l_next[l] / 2] == -1) {
            nxt = sylvan_ithvar(game.level_to_order[aag.l_next[l] / 2]);
        } else {
            nxt = game.s_gates[aag.lookup[aag.l_next[l] / 2]];
        }
        if (aag.l_next[l] & 1) nxt = sylvan_not(nxt);
        CV = sylvan_map_add(CV, game.level_to_order[aag.latches[l] / 2], nxt);
    }

    // now make output
    MTBDD Unsafe;
    mtbdd_protect(&Unsafe);
    if (aag.lookup[aag.outputs[0] / 2] == -1) {
        Unsafe = sylvan_ithvar(aag.outputs[0] / 2);
    } else {
        Unsafe = game.s_gates[aag.lookup[aag.outputs[0] / 2]];
    }
    if (aag.outputs[0] & 1) Unsafe = sylvan_not(Unsafe);
    Unsafe = sylvan_forall(Unsafe, game.c_inputs);
    Unsafe = sylvan_exists(Unsafe, game.u_inputs);


#if 0
    MTBDD supp = sylvan_support(Unsafe);
    while (supp != sylvan_set_empty()) {
        printf("%d ", sylvan_set_first(supp));
        supp = sylvan_set_next(supp);
    }
    printf("\n");
    INFO("exactly %.0f states are bad\n", sylvan_satcount(Unsafe, Lvars));
#endif

    MTBDD OldUnsafe = sylvan_false; // empty set
    MTBDD Step = sylvan_false;
    mtbdd_protect(&OldUnsafe);
    mtbdd_protect(&Step);

    while (Unsafe != OldUnsafe) {
        OldUnsafe = Unsafe;

        Step = sylvan_compose(Unsafe, CV);
        Step = sylvan_forall(Step, game.c_inputs);
        Step = sylvan_exists(Step, game.u_inputs);

        // check if initial state in Step (all 0)
        MTBDD Check = Step;
        while (Check != sylvan_false) {
            if (Check == sylvan_true) {
                return 0;
            } else {
                Check = sylvan_low(Check);
            }
        }

        Unsafe = sylvan_or(Unsafe, Step);
    }
    return 1;
}

int main(int argc, char **argv)
{
    t_start = wctime();
    setlocale(LC_NUMERIC, "en_US.utf-8");
    parse_args(argc, argv);
    INFO("Model: %s\n", filename);
    if (filename == nullptr) {
        Abort("Invalid file name.\n");
    }

    aag_buffer_open(&aag_buffer, filename, O_RDONLY);
    aag_file_read(&aag, &aag_buffer);

//    if (verbose) {
    if (0) {
        INFO("----------header----------\n");
        INFO("# of variables            \t %lu\n", aag.header.m);
        INFO("# of inputs               \t %lu\n", aag.header.i);
        INFO("# of latches              \t %lu\n", aag.header.l);
        INFO("# of outputs              \t %lu\n", aag.header.o);
        INFO("# of AND gates            \t %lu\n", aag.header.a);
        INFO("# of bad state properties \t %lu\n", aag.header.b);
        INFO("# of invariant constraints\t %lu\n", aag.header.c);
        INFO("# of justice properties   \t %lu\n", aag.header.j);
        INFO("# of fairness constraints \t %lu\n", aag.header.f);
        INFO("--------------------------\n");
    }

    DdManager *db = Cudd_Init(aag.header.m, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    Cudd_EnableReorderingReporting(db);

    // Init Lace
    lace_start(workers, 1000000); // auto-detect number of workers, use a 1,000,000 size task queue

    // Lace is initialized, now set local variables

    // Init Sylvan
    // Nodes table size of 1LL<<20 is 1048576 entries
    // Cache size of 1LL<<18 is 262144 entries
    // Nodes table size: 24 bytes * nodes
    // Cache table size: 36 bytes * cache entries
    // With 2^20 nodes and 2^18 cache entries, that's 33 MB
    // With 2^24 nodes and 2^22 cache entries, that's 528 MB
    sylvan_set_sizes(1LL<<14, 1LL<<18, 1LL<<14, 1LL<<18);
    sylvan_init_package();
    sylvan_set_granularity(2); // granularity 3 is decent value for this small problem - 1 means "use cache for every operation"
    sylvan_init_mtbdd();
    if (dynamic_reorder) sylvan_init_reorder();

    if (dynamic_reorder) sylvan_set_reorder_type(SYLVAN_REORDER_BOUNDED_SIFT);
    if (dynamic_reorder) sylvan_set_reorder_print(verbose);

    // Set hooks for logging garbage collection & dynamic variable reordering
    if (verbose) {
//        sylvan_re_hook_prere(TASK(reordering_start));
//        sylvan_re_hook_postre(TASK(reordering_end));
//        sylvan_gc_hook_pregc(TASK(gc_start));
//        sylvan_gc_hook_postgc(TASK(gc_end));
    }

    int is_realizable = solve_game(db);
    if (is_realizable) {
        INFO("REALIZABLE\n");
    } else {
        INFO("UNREALIZABLE\n");
    }

    // Report Sylvan statistics (if SYLVAN_STATS is set)
    if (verbose) sylvan_stats_report(stdout);

    aag_buffer_close(&aag_buffer);
    sylvan_quit();
    lace_stop();

    Cudd_Quit(db);

    return 0;
}

#include <getopt.h>

#include <sys/time.h>
#include <string>
#include <fcntl.h>
#include <span>

#include <sylvan_int.h>
#include <cuddInt.h>
#include "aag.h"


using namespace sylvan;

typedef struct cudd_safety_game
{
    DdNode      **gates;                // and gates
    DdNode      **c_inputs;             // controllable inputs
    DdNode      **u_inputs;             // uncontrollable inputs
} cudd_safety_game_t;

typedef struct sylvan_safety_game
{
    MTBDD  *gates;                      // Sylvan and gates
    MTBDD   c_inputs;                   // controllable inputs
    MTBDD   u_inputs;                   // uncontrollable inputs
} sylvan_safety_game_t;

double t_start;
#define INFO(s, ...) fprintf(stdout, "\r[% 8.2f] " s, wctime()-t_start, ##__VA_ARGS__)
#define Abort(s, ...) { fprintf(stderr, "\r[% 8.2f] " s, wctime()-t_start, ##__VA_ARGS__); exit(-1); }


/* Configuration */
static int workers = 1;
static int verbose = 0;
static char *filename = nullptr; // filename of the aag file
static int dynamic_reorder = 0;

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
        .content = nullptr,
        .size = 0,
        .pos = 0,
        .file_descriptor = -1,
        .filestat = {}
};

static sylvan_safety_game_t sylvan_game{
        .gates = nullptr,
        .c_inputs = sylvan_set_empty(),
        .u_inputs = sylvan_set_empty()
};

static cudd_safety_game_t cudd_game{
        .gates = nullptr,
        .c_inputs = nullptr,
        .u_inputs = nullptr
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
    if (sylvan_game.gates[gate] != sylvan_invalid) return;
    int lft = (int) aag.gatelft[gate] / 2;
    int rgt = (int) aag.gatergt[gate] / 2;

    DdNode *c_l, *c_r;
    MTBDD l, r;

    if (lft == 0) {
        l = sylvan_false;
        c_l = CUDD_FALSE;
    } else if (aag.lookup[lft] != -1) {
        make_gate(aag.lookup[lft], db);
        l = sylvan_game.gates[aag.lookup[lft]];
        c_l = cudd_game.gates[aag.lookup[lft]];
    } else {
        l = sylvan_ithvar(lft); // always use even variables (prime is odd)
        c_l = Cudd_bddIthVar(db, lft);
    }
    if (rgt == 0) {
        r = sylvan_false;
        c_r = CUDD_FALSE;
    } else if (aag.lookup[rgt] != -1) {
        make_gate(aag.lookup[rgt], db);
        r = sylvan_game.gates[aag.lookup[rgt]];
        c_r = cudd_game.gates[aag.lookup[rgt]];
    } else {
        r = sylvan_ithvar(rgt); // always use even variables (prime is odd)
        c_r = Cudd_bddIthVar(db, rgt);
    }
    if (aag.gatelft[gate] & 1) {
        l = sylvan_not(l);
        c_l = Cudd_Not(c_l);
    }
    if (aag.gatergt[gate] & 1) {
        r = sylvan_not(r);
        c_r = Cudd_Not(c_r);
    }

    sylvan_game.gates[gate] = sylvan_and(l, r);
    mtbdd_protect(&sylvan_game.gates[gate]);

    cudd_game.gates[gate] = Cudd_bddAnd(db, c_l, c_r);
    Cudd_Ref(cudd_game.gates[gate]);
}

#define solve_game(db) RUN(solve_game, db)
TASK_1(int, solve_game, DdManager*, db)
{
    sylvan_game.gates = new MTBDD[aag.header.a];
    for (uint64_t a = 0; a < aag.header.a; a++) sylvan_game.gates[a] = sylvan_invalid;
    cudd_game.gates = new DdNode*[aag.header.a];
    for (uint64_t a = 0; a < aag.header.a; a++) cudd_game.gates[a] = nullptr;

    INFO("Making the gate BDDs...\n");

    for (uint64_t gate = 0; gate < aag.header.a; gate++) {
        make_gate(gate, db);
//        sylvan_test_reduce_heap();
//        INFO("Sylvan: %5zu Cudd: %zu\n", llmsset_count_marked(nodes), Cudd_ReadNodeCount(db));
        if (llmsset_count_marked(nodes) >= reorder_db->config.size_threshold) {
            sylvan_reduce_heap(SYLVAN_REORDER_BOUNDED_SIFT);
            Cudd_ReduceHeap(db, CUDD_REORDER_SIFT, 0);
        }
    }

//    printf("Sylvan levels count:  %zu\n", levels_get_count(&reorder_db->levels));
//    printf("Cudd levels count:    %d\n", Cudd_ReadSize(db));
//
//    INFO("Sylvan gates have size %zu\n", mtbdd_nodecount_more(sylvan_game.gates, aag.header.a));
//    INFO("Cudd   gates have size %zu\n", Cudd_ReadNodeCount(db));
//
//    sylvan_reduce_heap(SYLVAN_REORDER_BOUNDED_SIFT);
//    Cudd_ReduceHeap(db, CUDD_REORDER_SIFT, 0);
//
//    INFO("Sylvan gates have size %zu\n", mtbdd_nodecount_more(sylvan_game.gates, aag.header.a));
//    INFO("Cudd   gates have size %zu\n", Cudd_ReadNodeCount(db));

    sylvan_game.c_inputs = sylvan_set_empty();
    sylvan_game.u_inputs = sylvan_set_empty();
    mtbdd_protect(&sylvan_game.c_inputs);
    mtbdd_protect(&sylvan_game.u_inputs);


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
                sylvan_game.c_inputs = sylvan_set_add(sylvan_game.c_inputs, aag.inputs[pos] / 2);
            } else {
                sylvan_game.u_inputs = sylvan_set_add(sylvan_game.u_inputs, aag.inputs[pos] / 2);
            }
        }
    }
    INFO("There are %zu controllable and %zu uncontrollable inputs.\n", sylvan_set_count(sylvan_game.c_inputs), sylvan_set_count(sylvan_game.u_inputs));

    // Actually just make the compose vector
    MTBDD CV = sylvan_map_empty();
    mtbdd_protect(&CV);

    for (uint64_t l = 0; l < aag.header.l; l++) {
        MTBDD nxt;
        if (aag.lookup[aag.l_next[l] / 2] == -1) {
            nxt = sylvan_ithvar(aag.l_next[l] / 2);
        } else {
            nxt = sylvan_game.gates[aag.lookup[aag.l_next[l] / 2]];
        }
        if (aag.l_next[l] & 1) nxt = sylvan_not(nxt);
        CV = sylvan_map_add(CV, aag.latches[l] / 2, nxt);
    }

    // now make output
    MTBDD Unsafe;
    mtbdd_protect(&Unsafe);
    if (aag.lookup[aag.outputs[0] / 2] == -1) {
        Unsafe = sylvan_ithvar(aag.outputs[0] / 2);
    } else {
        Unsafe = sylvan_game.gates[aag.lookup[aag.outputs[0] / 2]];
    }
    if (aag.outputs[0] & 1) Unsafe = sylvan_not(Unsafe);
    Unsafe = sylvan_forall(Unsafe, sylvan_game.c_inputs);
    Unsafe = sylvan_exists(Unsafe, sylvan_game.u_inputs);


    MTBDD OldUnsafe = sylvan_false; // empty set
    MTBDD Step = sylvan_false;
    mtbdd_protect(&OldUnsafe);
    mtbdd_protect(&Step);

    while (Unsafe != OldUnsafe) {
        OldUnsafe = Unsafe;

        Step = sylvan_compose(Unsafe, CV);
        Step = sylvan_forall(Step, sylvan_game.c_inputs);
        Step = sylvan_exists(Step, sylvan_game.u_inputs);

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

    if (verbose) {
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
    sylvan_set_sizes(1LL<<14, 1LL<<25, 1LL<<14, 1LL<<19);
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

#include "gempp_wrapper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>

#define VERSION "1.0.0"

static void print_usage(const char* program_name) {
    printf("Usage: %s [OPTIONS] <graph_file.txt>\n", program_name);
    printf("\n");
    printf("A subgraph matching implementation using binary linear programming,\n");
    printf("that attempts to find the subgraph of the target that best matches the\n");
    printf("query graph.\n");
    printf("\n");
    printf("Options:\n");
    printf("  -t, --tolerance LEVEL    Sets the tolerance level (default: t).\n");
    printf("                           e(xact)   -> not tolerant (perfect matching);\n");
    printf("                           l(abel)   -> tolerant to label errors;\n");
    printf("                           t(opology)-> tolerant to label and topology errors.\n");
    printf("  -i, --induced            Performs induced subgraph matching.\n");
    printf("  -s, --substitution FILE  Defines the weights for substitution costs.\n");
    printf("  -c, --creation FILE      Defines the weights for creation costs.\n");
    printf("  -n, --number NUM         Searches the 'n' best solutions (default: 1).\n");
    printf("  -e, --explore PERCENT    Upper bound approximation (0-100%%, default: 100).\n");
    printf("  --time LIMIT             Maximum solving time in seconds.\n");
    printf("  --solver SOLVER          Sets the solver (default: GLPK).\n");
    printf("  -p, --program FILE       Outputs the program to the given file.\n");
    printf("  -o, --solution FILE      Outputs the solution to the given file.\n");
    printf("  -v, --verbose            Enables verbose output.\n");
    printf("  -h, --help               Shows this help message.\n");
    printf("  --version                Shows version information.\n");
    printf("\n");
    printf("The input file must be a .txt file containing adjacency matrices for\n");
    printf("two graphs in the format used by graph matching competitions.\n");
}

static void print_version() {
    printf("gempp-sub version %s\n", VERSION);
    printf("Built with libgem++ and GLPK\n");
}

int main(int argc, char* argv[]) {
    // Default configuration
    GemppTolerance tolerance = GEMPP_TOLERANCE_TOPOLOGY;
    bool induced = false;
    const char* substitution_file = NULL;
    const char* creation_file = NULL;
    int number_solutions = 1;
    double upper_bound = 1.0;
    double time_limit = 0.0;
    const char* solver = "GLPK";
    const char* program_file = NULL;
    const char* solution_file = NULL;
    bool verbose = false;

    // Command line options
    static struct option long_options[] = {
        {"tolerance", required_argument, 0, 't'},
        {"induced", no_argument, 0, 'i'},
        {"substitution", required_argument, 0, 's'},
        {"creation", required_argument, 0, 'c'},
        {"number", required_argument, 0, 'n'},
        {"explore", required_argument, 0, 'e'},
        {"time", required_argument, 0, 0},
        {"solver", required_argument, 0, 0},
        {"program", required_argument, 0, 'p'},
        {"solution", required_argument, 0, 'o'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 0},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "t:is:c:n:e:p:o:vh", long_options, &option_index)) != -1) {
        switch (c) {
            case 't':
                if (strcmp(optarg, "e") == 0 || strcmp(optarg, "exact") == 0) {
                    tolerance = GEMPP_TOLERANCE_EXACT;
                } else if (strcmp(optarg, "l") == 0 || strcmp(optarg, "label") == 0) {
                    tolerance = GEMPP_TOLERANCE_LABEL;
                } else if (strcmp(optarg, "t") == 0 || strcmp(optarg, "topology") == 0) {
                    tolerance = GEMPP_TOLERANCE_TOPOLOGY;
                } else {
                    fprintf(stderr, "Invalid tolerance level: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'i':
                induced = true;
                break;
            case 's':
                substitution_file = optarg;
                break;
            case 'c':
                creation_file = optarg;
                break;
            case 'n':
                number_solutions = atoi(optarg);
                if (number_solutions < 1) {
                    fprintf(stderr, "Number of solutions must be positive\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'e':
                upper_bound = atof(optarg) / 100.0;
                if (upper_bound < 0.0 || upper_bound > 1.0) {
                    fprintf(stderr, "Upper bound must be between 0 and 100%%\n");
                    return EXIT_FAILURE;
                }
                break;
            case 'p':
                program_file = optarg;
                break;
            case 'o':
                solution_file = optarg;
                break;
            case 'v':
                verbose = true;
                break;
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case 0:  // Long options without short equivalents
                if (strcmp(long_options[option_index].name, "time") == 0) {
                    time_limit = atof(optarg);
                } else if (strcmp(long_options[option_index].name, "solver") == 0) {
                    solver = optarg;
                } else if (strcmp(long_options[option_index].name, "version") == 0) {
                    print_version();
                    return EXIT_SUCCESS;
                }
                break;
            case '?':
                return EXIT_FAILURE;
            default:
                abort();
        }
    }

    // Check remaining arguments
    if (optind >= argc) {
        fprintf(stderr, "Error: No input file specified\n\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (optind < argc - 1) {
        fprintf(stderr, "Error: Too many arguments\n\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char* input_file = argv[optind];

    // Check file extension
    const char* ext = strrchr(input_file, '.');
    if (!ext || strcmp(ext, ".txt") != 0) {
        fprintf(stderr, "Error: Input file must have .txt extension\n");
        return EXIT_FAILURE;
    }

    if (verbose) {
        printf("Parsing adjacency matrix file: %s\n", input_file);
    }

    // Parse the input file
    GemppGraphs* graphs = gempp_parse_adjacency_matrix_file(input_file);
    if (!graphs) {
        fprintf(stderr, "Error: Failed to parse input file\n");
        return EXIT_FAILURE;
    }

    // Create configuration
    GemppConfiguration* config = gempp_configuration_create();
    if (!config) {
        fprintf(stderr, "Error: Failed to create configuration\n");
        gempp_graphs_free(graphs);
        return EXIT_FAILURE;
    }

    // Set configuration options
    gempp_configuration_set_solver(config, solver);
    gempp_configuration_set_tolerance(config, tolerance);
    gempp_configuration_set_induced(config, induced);
    gempp_configuration_set_number_solutions(config, number_solutions);
    gempp_configuration_set_upper_bound(config, upper_bound);
    gempp_configuration_set_time_limit(config, time_limit);
    gempp_configuration_set_verbose(config, verbose);

    if (program_file) {
        gempp_configuration_set_program_output(config, program_file);
    }
    if (solution_file) {
        gempp_configuration_set_solution_output(config, solution_file);
    }
    if (substitution_file) {
        gempp_configuration_set_substitution_weights(config, substitution_file);
    }
    if (creation_file) {
        gempp_configuration_set_creation_weights(config, creation_file);
    }

    // Create and run matcher
    GemppMatcher* matcher = gempp_matcher_create(graphs, config);
    if (!matcher) {
        fprintf(stderr, "Error: Failed to create matcher\n");
        gempp_configuration_free(config);
        gempp_graphs_free(graphs);
        return EXIT_FAILURE;
    }

    if (verbose) {
        printf("Running subgraph matching...\n");
    }

    double objective = gempp_matcher_run(matcher);

    if (objective >= 0.0) {
        printf("Matching completed. Objective value: %.6f\n", objective);
    } else {
        fprintf(stderr, "Error: Matching failed\n");
    }

    // Cleanup
    gempp_matcher_free(matcher);
    gempp_configuration_free(config);
    gempp_graphs_free(graphs);

    return objective >= 0.0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
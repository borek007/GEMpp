#ifndef GEMPP_WRAPPER_H
#define GEMPP_WRAPPER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque types for C API
typedef struct GemppGraphs GemppGraphs;
typedef struct GemppConfiguration GemppConfiguration;
typedef struct GemppMatcher GemppMatcher;

// Tolerance levels for subgraph matching
typedef enum {
    GEMPP_TOLERANCE_EXACT = 0,
    GEMPP_TOLERANCE_LABEL = 1,
    GEMPP_TOLERANCE_TOPOLOGY = 2
} GemppTolerance;

// Cut methods
typedef enum {
    GEMPP_CUT_SOLUTION = 0,
    GEMPP_CUT_MATCHING = 1,
    GEMPP_CUT_ELEMENTS = 2
} GemppCutMethod;

// Function declarations
GemppGraphs* gempp_parse_adjacency_matrix_file(const char* filename);
void gempp_graphs_free(GemppGraphs* graphs);

GemppConfiguration* gempp_configuration_create();
void gempp_configuration_free(GemppConfiguration* config);
void gempp_configuration_set_solver(GemppConfiguration* config, const char* solver);
void gempp_configuration_set_tolerance(GemppConfiguration* config, GemppTolerance tolerance);
void gempp_configuration_set_induced(GemppConfiguration* config, bool induced);
void gempp_configuration_set_number_solutions(GemppConfiguration* config, int number);
void gempp_configuration_set_upper_bound(GemppConfiguration* config, double upper_bound);
void gempp_configuration_set_time_limit(GemppConfiguration* config, double time_limit);
void gempp_configuration_set_verbose(GemppConfiguration* config, bool verbose);
void gempp_configuration_set_program_output(GemppConfiguration* config, const char* filename);
void gempp_configuration_set_solution_output(GemppConfiguration* config, const char* filename);
void gempp_configuration_set_substitution_weights(GemppConfiguration* config, const char* filename);
void gempp_configuration_set_creation_weights(GemppConfiguration* config, const char* filename);

GemppMatcher* gempp_matcher_create(GemppGraphs* graphs, GemppConfiguration* config);
void gempp_matcher_free(GemppMatcher* matcher);
double gempp_matcher_run(GemppMatcher* matcher);

#ifdef __cplusplus
}
#endif

#endif /* GEMPP_WRAPPER_H */
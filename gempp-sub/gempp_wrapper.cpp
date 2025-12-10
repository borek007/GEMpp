#include "gempp_wrapper.h"
#include <QString>
#include <QDebug>
#include <QCoreApplication>

// Include GEM++ headers
#include "Model/AdjacencyMatrixParser.h"
#include "Model/Problem.h"
#include "Model/Weights.h"
#include "Solver/Configuration.h"
#include "Solver/Matcher.h"

// Qt application instance
static QCoreApplication* qtApp = nullptr;
static int qtArgc = 1;
static char* qtArgv[] = {"gempp-sub"};

struct GemppGraphs {
    Graph* query;
    Graph* target;

    GemppGraphs(Graph* q, Graph* t) : query(q), target(t) {}
    ~GemppGraphs() {
        delete query;
        delete target;
    }
};

struct GemppConfiguration {
    Configuration* config;

    GemppConfiguration() : config(new Configuration()) {
        // Set defaults
        config->matchingType = Problem::SUBGRAPH;
        config->solver = "GLPK";
        config->subMethod = SubgraphMatching::TOPOLOGY;
        config->induced = false;
        config->number = 1;
        config->upperbound = 1.0;
        config->timeLimit = 0.0;
        config->verbose = false;
        config->threadsPerInstance = 1;
        config->parallelInstances = 1;
        config->cutMethod = Formulation::SOLUTION;
    }

    ~GemppConfiguration() {
        delete config;
    }
};

struct GemppMatcher {
    Problem* problem;
    Weights* weights;
    Configuration* config;
    Matcher* matcher;

    GemppMatcher(Graph* query, Graph* target, Configuration* cfg)
        : problem(new Problem(Problem::SUBGRAPH, query, target)),
          weights(new Weights()),
          config(cfg),
          matcher(new Matcher(problem, config)) {}

    ~GemppMatcher() {
        delete matcher;
        delete weights;
        delete problem;
    }
};

extern "C" {

GemppGraphs* gempp_parse_adjacency_matrix_file(const char* filename) {
    // Initialize Qt if not already done
    if (!qtApp) {
        qtApp = new QCoreApplication(qtArgc, qtArgv);
    }

    try {
        auto graphs = AdjacencyMatrixParser::parseFile(QString::fromUtf8(filename));
        return new GemppGraphs(graphs.first, graphs.second);
    } catch (const std::exception& e) {
        qCritical() << "Error parsing file:" << e.what();
        return nullptr;
    }
}

void gempp_graphs_free(GemppGraphs* graphs) {
    delete graphs;
}

GemppConfiguration* gempp_configuration_create() {
    return new GemppConfiguration();
}

void gempp_configuration_free(GemppConfiguration* config) {
    delete config;
}

void gempp_configuration_set_solver(GemppConfiguration* config, const char* solver) {
    config->config->solver = QString::fromUtf8(solver);
}

void gempp_configuration_set_tolerance(GemppConfiguration* config, GemppTolerance tolerance) {
    switch (tolerance) {
        case GEMPP_TOLERANCE_EXACT:
            config->config->subMethod = SubgraphMatching::EXACT;
            break;
        case GEMPP_TOLERANCE_LABEL:
            config->config->subMethod = SubgraphMatching::LABEL;
            break;
        case GEMPP_TOLERANCE_TOPOLOGY:
            config->config->subMethod = SubgraphMatching::TOPOLOGY;
            break;
    }
}

void gempp_configuration_set_induced(GemppConfiguration* config, bool induced) {
    config->config->induced = induced;
}

void gempp_configuration_set_number_solutions(GemppConfiguration* config, int number) {
    config->config->number = number;
}

void gempp_configuration_set_upper_bound(GemppConfiguration* config, double upper_bound) {
    config->config->upperbound = upper_bound;
}

void gempp_configuration_set_time_limit(GemppConfiguration* config, double time_limit) {
    config->config->timeLimit = time_limit;
}

void gempp_configuration_set_verbose(GemppConfiguration* config, bool verbose) {
    config->config->verbose = verbose;
}

void gempp_configuration_set_program_output(GemppConfiguration* config, const char* filename) {
    config->config->program = QString::fromUtf8(filename);
}

void gempp_configuration_set_solution_output(GemppConfiguration* config, const char* filename) {
    config->config->solution = QString::fromUtf8(filename);
}

void gempp_configuration_set_substitution_weights(GemppConfiguration* config, const char* filename) {
    config->config->substitution = QString::fromUtf8(filename);
}

void gempp_configuration_set_creation_weights(GemppConfiguration* config, const char* filename) {
    config->config->creation = QString::fromUtf8(filename);
}

GemppMatcher* gempp_matcher_create(GemppGraphs* graphs, GemppConfiguration* config) {
    try {
        return new GemppMatcher(graphs->query, graphs->target, config->config);
    } catch (const std::exception& e) {
        qCritical() << "Error creating matcher:" << e.what();
        return nullptr;
    }
}

void gempp_matcher_free(GemppMatcher* matcher) {
    delete matcher;
}

double gempp_matcher_run(GemppMatcher* matcher) {
    try {
        // Compute costs if weights are available
        if (!matcher->config->substitution.isEmpty() || !matcher->config->creation.isEmpty()) {
            matcher->weights = new Weights(matcher->config->substitution, matcher->config->creation);
        }
        matcher->problem->computeCosts(matcher->weights);

        // Run the matcher
        matcher->matcher->run();
        return matcher->matcher->getObjective();
    } catch (const std::exception& e) {
        qCritical() << "Error running matcher:" << e.what();
        return -1.0;
    }
}

} // extern "C"
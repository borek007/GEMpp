#ifndef GEMPP_CONFIGURATION_H
#define GEMPP_CONFIGURATION_H

// Minimal stub for Qt-free build
#include <string>

class DLL_EXPORT Configuration {
    public:
        Configuration() : verbose(false), timeLimit(0) {}
        virtual ~Configuration() {}

        bool verbose;
        double timeLimit;
        std::string outputDir;

        // GLPK-specific configuration
        int msg_lev;
        double tm_lim;
        double mip_gap;
        int presolve;
};

#endif /* GEMPP_CONFIGURATION_H */
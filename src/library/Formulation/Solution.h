#ifndef GEMPP_SOLUTION_H
#define GEMPP_SOLUTION_H

// Minimal stub for Qt-free build
#include <unordered_map>
#include <string>

class Variable;

class DLL_EXPORT Solution {
    public:
        enum Status {
            NOT_SOLVED = 0,
            OPTIMAL,
            SUBOPTIMAL,
            INFEASIBLE
        };

        Solution() : objective_(0), status_(NOT_SOLVED) {}
        virtual ~Solution() {}

        void clean() {}
        void setStatus(Status status) { status_ = status; }
        void setObjective(double obj) { objective_ = obj; }
        void addVariable(Variable *var, double value) {}

        Status getStatus() const { return status_; }
        double getObjective() const { return objective_; }

    private:
        double objective_;
        Status status_;
        std::unordered_map<std::string, double> variables_;
};

#endif /* GEMPP_SOLUTION_H */
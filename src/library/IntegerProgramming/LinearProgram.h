#ifndef GEMPP_LINEARPROGRAM_H
#define GEMPP_LINEARPROGRAM_H

// Minimal stub for Qt-free build
#include <vector>
#include <unordered_map>
#include <string>
#include "Program.h"

class Variable;
class LinearConstraint;
class LinearExpression;

class DLL_EXPORT LinearProgram : public Program {
    public:
        enum Sense {
            MINIMIZE = 0,
            MAXIMIZE
        };

        LinearProgram() : sense_(MINIMIZE) {}
        virtual ~LinearProgram() {}

        Sense getSense() const { return sense_; }
        void setSense(Sense sense) { sense_ = sense; }

        LinearExpression *getObjective() { return nullptr; }  // Stub
        std::vector<Variable*> getVariables() { return std::vector<Variable*>(); }  // Stub
        std::vector<LinearConstraint*> getConstraints() { return std::vector<LinearConstraint*>(); }  // Stub

        Variable *getVariable(const std::string &id) { return nullptr; }  // Stub

    private:
        Sense sense_;
};

#endif /* GEMPP_LINEARPROGRAM_H */
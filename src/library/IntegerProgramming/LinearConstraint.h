#ifndef GEMPP_LINEARCONSTRAINT_H
#define GEMPP_LINEARCONSTRAINT_H

// Minimal stub for Qt-free build
#include <string>

class LinearExpression;

class DLL_EXPORT LinearConstraint {
    public:
        enum Relation {
            LESS_EQ = 0,
            GREATER_EQ,
            EQUAL
        };

        LinearConstraint() : relation_(LESS_EQ), rhs_(0) {}
        virtual ~LinearConstraint() {}

        Relation getRelation() const { return relation_; }
        double getRHS() const { return rhs_; }
        LinearExpression *getLinearExpression() { return nullptr; }  // Stub

        const std::string &getID() const { return id_; }

    private:
        std::string id_;
        Relation relation_;
        double rhs_;
};

#endif /* GEMPP_LINEARCONSTRAINT_H */
#ifndef GEMPP_LINEAREXPRESSION_H
#define GEMPP_LINEAREXPRESSION_H

#include <unordered_map>
#include "Variable.h"
#include "Expression.h"

/**
 * @brief The LinearExpression class represents the mathematical concept of linear expressions, used in LinearConstraint.
 */
class DLL_EXPORT LinearExpression : virtual public Expression {
    public:
        LinearExpression() {}
        virtual ~LinearExpression() {}

        std::unordered_map<Variable*, double> &getTerms() { return terms_; }
        size_t size() const { return terms_.size(); }

    private:
        std::unordered_map<Variable*, double> terms_;
};

#endif /* GEMPP_LINEAREXPRESSION_H */

#ifndef GEMPP_EXPRESSION_H
#define GEMPP_EXPRESSION_H

#include "Core/IPrintable.h"

/**
 * @brief The Expression class represents the mathematical expression of a Constraint.
 */
class DLL_EXPORT Expression : virtual public IPrintable {
    public:
        Expression() : const_(0) {}
        virtual ~Expression() {}

        double getConst() { return const_; }
        void addConst(double d) { const_ += d; }
        void multiplyBy(double d) { const_ *= d; }
        double eval() { return const_; }

    protected:
        double const_;
};

#endif /* GEMPP_EXPRESSION_H */

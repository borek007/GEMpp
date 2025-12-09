/*  GEM++GLPK : a solver plugin for GEM++ based on GLPK
    Copyright (C) 2013-2015 LITIS laboratory

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#ifndef GLPKPLUGIN_SIMPLE_H
#define GLPKPLUGIN_SIMPLE_H

#include <unordered_map>
#include <string>
#include <cmath>
#include "glpk.h"
#include "Solver/Solver.h"
#include "Solver/Configuration.h"
#include "Formulation/Solution.h"
#include "IntegerProgramming/LinearProgram.h"
#include "IntegerProgramming/LinearExpression.h"
#include "IntegerProgramming/LinearConstraint.h"
#include "Core/Math.h"

class GLPK : public Solver{
    public:
        GLPK();
        ~GLPK();

        void init(Configuration *cfg);
        void init(LinearProgram *lp, Configuration *cfg = 0);
        double solve(Solution *sol = 0);
        void update(bool newBounds = false);

    private:
        glp_prob *model_;
        glp_iocp config_;
        std::unordered_map<std::string, int> varOrder_;
        std::unordered_map<std::string, int> constOrder_;
        int *ia_, *ja_;
        double *ar_;
        int nz_;

        void addVar(Variable *v);
        void addLinearConstraint(LinearConstraint *c);
        void addQuadConstraint(QuadConstraint *c);
        void setObjective();
        void prepare();
        void initMatrix();
};

#endif /* GLPKPLUGIN_SIMPLE_H */

// Reaktor is a C++ library for computational reaction modelling.
//
// Copyright (C) 2014 Allan Leal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// Reaktor includes
#include <Reaktor/Common/Matrix.hpp>

namespace Reaktor {

/// A type that describes the solution of an optimisation problem
struct OptimumSolution
{
    /// The primal solution of the optimisation problem
    Vector x;

    /// The dual solution of the optimisation problem with respect to the equality constraints
    Vector y;

    /// The dual solution of the optimisation problem with respect to the bound constraints
    Vector z;
};

/// A type that describes the result of an optimisation calculation
struct OptimumStatistics
{
    /// The flag that indicates if the optimisation calculation converged
    bool converged = false;

    /// The number of iterations in the optimisation calculation
    unsigned num_iterations = 0;

    /// The number of evaluations of the objective function in the optimisation calculation
    unsigned num_objective_evals = 0;

    /// The convergence rate of the optimisation calculation near the solution
    double convergence_rate = 0;

    /// The final residual error of the optimisation calculation
    double error = 0;

    /// The wall time spent for the optimisation calculation (in units of s)
    double time = 0;

    /// The wall time spent for all objective evaluations (in units of s)
    double time_objective_evals = 0;

    /// The wall time spent for all contraint evaluations (in units of s)
    double time_constraint_evals = 0;

    /// The wall time spent for all linear system solutions (in units of s)
    double time_linear_system = 0;
};

/// A type that describes the result of an optimisation calculation
struct OptimumResult
{
    /// The solution of the optimisation calculation
    OptimumSolution solution;

    /// The statistics of the optimisation calculation
    OptimumStatistics statistics;
};

} // namespace Reaktor

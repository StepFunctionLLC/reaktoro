// Reaktoro is a unified framework for modeling chemically reactive systems.
//
// Copyright © 2014-2022 Allan Leal
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library. If not, see <http://www.gnu.org/licenses/>.

// pybind11 includes
#include <Reaktoro/pybind11.hxx>

// Reaktoro includes
#include <Reaktoro/Core/ChemicalState.hpp>
#include <Reaktoro/Core/ChemicalSystem.hpp>
#include <Reaktoro/Equilibrium/EquilibriumConditions.hpp>
#include <Reaktoro/Equilibrium/EquilibriumRestrictions.hpp>
#include <Reaktoro/Kinetics/KineticsSensitivity.hpp>
#include <Reaktoro/Kinetics/SmartKineticsOptions.hpp>
#include <Reaktoro/Kinetics/SmartKineticsResult.hpp>
#include <Reaktoro/Kinetics/SmartKineticsSolver.hpp>
using namespace Reaktoro;

void exportSmartKineticsSolver(py::module& m)
{
    py::class_<SmartKineticsSolver>(m, "SmartKineticsSolver")
        .def(py::init<ChemicalSystem const&>())
        .def(py::init<EquilibriumSpecs const&>())

        .def("solve", py::overload_cast<ChemicalState&, real const&>(&SmartKineticsSolver::solve), "React a chemical state for a given time interval.", py::arg("state"), py::arg("dt"))
        .def("solve", py::overload_cast<ChemicalState&, real const&, EquilibriumRestrictions const&>(&SmartKineticsSolver::solve), "React a chemical state for a given time interval respecting given reactivity restrictions.", py::arg("state"), py::arg("dt"), py::arg("restrictions"))
        .def("solve", py::overload_cast<ChemicalState&, real const&, EquilibriumConditions const&>(&SmartKineticsSolver::solve), "React a chemical state for a given time interval respecting given constraint conditions.", py::arg("state"), py::arg("dt"), py::arg("conditions"))
        .def("solve", py::overload_cast<ChemicalState&, real const&, EquilibriumConditions const&, EquilibriumRestrictions const&>(&SmartKineticsSolver::solve), "React a chemical state for a given time interval respecting given constraint conditions and reactivity restrictions.", py::arg("state"), py::arg("dt"), py::arg("conditions"), py::arg("restrictions"))

        .def("solve", py::overload_cast<ChemicalState&, KineticsSensitivity&, real const&>(&SmartKineticsSolver::solve), "React a chemical state for a given time interval and compute sensitivity derivatives.", py::arg("state"), py::arg("sensitivity"), py::arg("dt"))
        .def("solve", py::overload_cast<ChemicalState&, KineticsSensitivity&, real const&, EquilibriumRestrictions const&>(&SmartKineticsSolver::solve), "React a chemical state for a given time interval respecting given reactivity restrictions and compute sensitivity derivatives.", py::arg("state"), py::arg("sensitivity"), py::arg("dt"), py::arg("restrictions"))
        .def("solve", py::overload_cast<ChemicalState&, KineticsSensitivity&, real const&, EquilibriumConditions const&>(&SmartKineticsSolver::solve), "React a chemical state for a given time interval respecting given constraint conditions and compute sensitivity derivatives.", py::arg("state"), py::arg("sensitivity"), py::arg("dt"), py::arg("conditions"))
        .def("solve", py::overload_cast<ChemicalState&, KineticsSensitivity&, real const&, EquilibriumConditions const&, EquilibriumRestrictions const&>(&SmartKineticsSolver::solve), "React a chemical state for a given time interval respecting given constraint conditions and reactivity restrictions and compute sensitivity derivatives.", py::arg("state"), py::arg("sensitivity"), py::arg("dt"), py::arg("conditions"), py::arg("restrictions"))

        .def("setOptions", &SmartKineticsSolver::setOptions)
        ;
}

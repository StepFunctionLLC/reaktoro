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
#include <Reaktoro/Models/ActivityModels/ActivityModelCubicEOS.hpp>
using namespace Reaktoro;

void exportActivityModelCubicEOS(py::module& m)
{
    m.def("ActivityModelVanDerWaals", ActivityModelVanDerWaals,
        "Return the activity model for fluid phases based on the Van der Waals cubic equation of state.",
            py::arg("cbipmodel") = CubicBipModelGenerator());

    m.def("ActivityModelRedlichKwong", ActivityModelRedlichKwong,
        "Return the activity model for fluid phases based on the Redlich-Kwong cubic equation of state.",
            py::arg("cbipmodel") = CubicBipModelGenerator());

    m.def("ActivityModelSoaveRedlichKwong", ActivityModelSoaveRedlichKwong,
        "Return the activity model for fluid phases based on the Soave-Redlich-Kwong cubic equation of state.",
            py::arg("cbipmodel") = CubicBipModelGenerator());

    m.def("ActivityModelPengRobinson", ActivityModelPengRobinson,
        "Return the activity model for fluid phases based on the Peng-Robinson (1978) cubic equation of state.",
            py::arg("cbipmodel") = CubicBipModelGenerator());

    m.def("ActivityModelPengRobinson76", ActivityModelPengRobinson76,
        "Return the activity model for fluid phases based on the Peng-Robinson (1976) cubic equation of state.",
            py::arg("cbipmodel") = CubicBipModelGenerator());

    m.def("ActivityModelPengRobinson78", ActivityModelPengRobinson78,
        "Return the activity model for fluid phases based on the Peng-Robinson (1978) cubic equation of state.",
            py::arg("cbipmodel") = CubicBipModelGenerator());

    m.def("ActivityModelPengRobinsonPhreeqc", ActivityModelPengRobinsonPhreeqc,
        "Return the activity model for fluid phases based on the Peng-Robinson (1976) with the binary interaction parameter model used in PHREEQC.");

    m.def("ActivityModelPengRobinsonSoreideWhitson", ActivityModelPengRobinsonSoreideWhitson,
        "Return the activity model for fluid phases based on the Peng-Robinson (1978) with the binary interaction parameter model of Søreide and Whitson (1992).");

    m.def("CubicBipModelPhreeqc", CubicBipModelPhreeqc,
        "Return the binary interaction parameter model for Peng-Robinson EOS (1976) equivalent to that used in PHREEQC.");

    m.def("CubicBipModelSoreideWhitson", CubicBipModelSoreideWhitson,
        "Return the binary interaction parameter model for Peng-Robinson EOS (1978) equivalent to that reported in Søreide and Whitson (1992).");

    // DEPRECATED METHODS THAT WILL BE REMOVED IN THE FUTURE

    m.def("ActivityModelPengRobinsonPHREEQC", []() -> ActivityModelGenerator { errorif(true, "ActivityModelPengRobinsonPHREEQC has been renamed to ActivityModelPengRobinsonPhreeqc. Please make this change in your code."); return {}; },
        "ActivityModelPengRobinsonPHREEQC has been renamed to ActivityModelPengRobinsonPhreeqc. Please make this change in your code.");

    m.def("CubicBipModelPHREEQC", []() -> ActivityModelGenerator { errorif(true, "ActivityModelPengRobinsonPHREEQC has been renamed to ActivityModelPengRobinsonPhreeqc. Please make this change in your code."); return {}; },
        "ActivityModelPengRobinsonPHREEQC has been renamed to ActivityModelPengRobinsonPhreeqc. Please make this change in your code.");
}

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

// -----------------------------------------------------------------------------
// 👏 Acknowledgements 👏
// -----------------------------------------------------------------------------
// This example was originally authored by:
//   • Svetlana Kyas (30 August 2021)
//
// and since revised by:
//   • Allan Leal (28 August 2023)
//     - Using ActivityModelPhreeqc instead of ActivityModelHKF for aqueous phase.
//   • Allan Leal (16 July 2021)
// -----------------------------------------------------------------------------

#include <Reaktoro/Reaktoro.hpp>

using namespace Reaktoro;

int main()
{
    // Initialize the Phreeqc database
    PhreeqcDatabase db("phreeqc.dat");

    // Define an aqueous phase
    AqueousPhase aqueousphase(speciate("H O C Ca Na Mg Cl"));
    aqueousphase.set(ActivityModelPhreeqc(db));

    // Define an ion exchange phase
    IonExchangePhase exchangephase("NaX CaX2 MgX2");
    exchangephase.setActivityModel(ActivityModelIonExchangeGainesThomas());

    // Construct the chemical system
    ChemicalSystem system(db, aqueousphase, exchangephase);

    const auto T = 25.0; // temperature in celsius
    const auto P = 1.0;  // pressure in atm

    // Define initial equilibrium state
    ChemicalState solutionstate(system);
    solutionstate.temperature(T, "celsius");
    solutionstate.pressure(P, "atm");
    solutionstate.set("H2O"    , 1.00, "kg");

    // Scale solution recipe to match the values of the PHREEQC examples
    solutionstate.set("Na+"  , 1.10, "mol");
    solutionstate.set("Mg+2" , 0.48, "mol");
    solutionstate.set("Ca+2" , 1.90, "mol");

    // Set the number of exchange assuming that it is completely occupied by Na
    solutionstate.set("NaX"  , 0.06, "umol");

    // Create an equilibrium solver and equilibrate given initial state
    EquilibriumSolver solver(system);
    solver.solve(solutionstate);

    // Output the chemical state to a text file
    solutionstate.output("state.txt");
    std::cout << solutionstate << std::endl;

    AqueousProps aprops(solutionstate);
    std::cout << "I  = " << aprops.ionicStrength() << " mol/kgw" << std::endl;
    std::cout << "pH = " << aprops.pH() << std::endl;
    std::cout << "pE = " << aprops.pE() << std::endl;

    ChemicalProps chemprops(solutionstate);
    IonExchangeProps exprops(chemprops);
    std::cout << exprops << std::endl;

    return 0;
}

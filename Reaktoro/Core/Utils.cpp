// Reaktoro is a C++ library for computational reaction modelling.
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

#include "Utils.hpp"

// Reaktoro includes
#include <Reaktoro/Common/ChemicalScalar.hpp>
#include <Reaktoro/Common/ChemicalVector.hpp>

namespace Reaktoro {

auto molarFractions(const Vector& n_) -> ChemicalVector
{
    auto n = composition(n_);
    const unsigned nspecies = n.size();
    if(nspecies == 1)
        return ChemicalVector(ones(1), zeros(1), zeros(1), ones(1));
    const ChemicalScalar nt = sum(n);
    return (nt.val != 0.0) ? n/nt : ChemicalVector(nspecies);
}

} // namespace Reaktoro

// Reaktoro is a unified framework for modeling chemically reactive systems.
//
// Copyright © 2014-2021 Allan Leal
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

#pragma once

// Reaktoro includes
#include <Reaktoro/Common/TraitsUtils.hpp>
#include <Reaktoro/Common/Matrix.hpp>
#include <Reaktoro/Thermodynamics/Water/WaterConstants.hpp>

namespace Reaktoro {

/// Compute the molalities of the species with given species amounts.
/// @param n The vector with the species amounts.
/// @param iH2O The index of the water solvent species.
/// @param[out] m The vector of species molalities.
template<typename ArrayConstRef, typename ArrayRef>
auto molalities(ArrayConstRef&& n, Index iH2O, ArrayRef&& m)
{
    const auto kgH2O = n[iH2O] * waterMolarMass;
    const auto nspecies = n.size();
    assert(iH2O < nspecies);
    if(nspecies == 1) {
        m[iH2O] = 1.0/waterMolarMass;
        return;
    }
    if(kgH2O != 0.0) m = n/kgH2O;
    else
    {
        m.fill(0.0);
        m[iH2O] = 1.0/waterMolarMass;
    }
}

/// Compute the molalities of the species with given species amounts.
/// @param n The vector with the species amounts.
/// @param iH2O The index of the water solvent species.
template<typename ArrayConstRef>
auto molalities(ArrayConstRef&& n, Index iH2O)
{
    using T = Decay<decltype(n[0])>;
    const auto N = n.size();
    ArrayX<T> m(N);
    molalities(n, iH2O, m);
    return m;
}

/// Compute the Jacobian matrix of the species molalities (@eq{J=\frac{\partial m}{\partial n}}).
/// @param n The vector with the species amounts.
/// @param iH2O The index of the water solvent species.
/// @param[out] J The output Jacobian matrix.
template<typename ArrayConstRef, typename MatrixRef>
auto molalitiesJacobian(ArrayConstRef&& n, Index iH2O, MatrixRef&& J)
{
    //-----------------------------------------------------------------------------------------------------
    // == LATEX ==
    //-----------------------------------------------------------------------------------------------------
    // \frac{\partial m_{i}}{\partial n_{j}}=m_{i}\left(\frac{\delta_{ij}}{n_{i}}-\frac{\delta_{jw}}{n_{w}}\right)
    //-----------------------------------------------------------------------------------------------------
    const auto N = n.size();
    assert(J.rows() == N);
    assert(J.cols() == N);
    J.fill(0.0);
    const auto nH2O = n[iH2O];
    if(nH2O == 0.0)
        return;
    const auto kgH2O = nH2O * waterMolarMass;
    const auto kgH2Oinv = 1.0/kgH2O;
    for(auto i = 0; i < N; ++i)
    {
        const auto mi = n[i]/kgH2O;
        J(i, i) = kgH2Oinv;
        J(i, iH2O) -= mi/nH2O;
    }
    J(iH2O, iH2O) = 0.0; // ensure sharp zero here as the above computation could produce round-off errors for this entry
}

/// Compute the Jacobian matrix of the species molalities (@eq{J=\frac{\partial m}{\partial n}}).
/// @param n The vector with the species amounts.
/// @param iH2O The index of the water solvent species.
template<typename ArrayConstRef>
auto molalitiesJacobian(ArrayConstRef&& n, Index iH2O)
{
    using T = Decay<decltype(n[0])>;
    const auto N = n.size();
    MatrixX<T> J(N, N);
    molalitiesJacobian(n, iH2O, J);
    return J;
}

/// Compute the Jacobian matrix of the species molalities in natural log (@eq{J=\frac{\partial\ln m}{\partial n}}).
/// @param n The vector with the species amounts.
/// @param iH2O The index of the water solvent species.
/// @param[out] J The output Jacobian matrix.
template<typename ArrayConstRef, typename MatrixRef>
auto lnMolalitiesJacobian(ArrayConstRef&& n, Index iH2O, MatrixRef&& J) -> void
{
    //-----------------------------------------------------------------------------------------------------
    // == LATEX ==
    //-----------------------------------------------------------------------------------------------------
    // \frac{\partial\ln m_{i}}{\partial n_{j}}=\frac{1}{m_{i}}\frac{\partial m_{i}}{\partial n_{j}}=\left(\frac{\delta_{ij}}{n_{i}}-\frac{\delta_{jw}}{n_{w}}\right)
    //-----------------------------------------------------------------------------------------------------
    const auto N = n.size();
    assert(J.rows() == N);
    assert(J.cols() == N);
    J.fill(0.0);
    const auto nH2O = n[iH2O];
    if(nH2O == 0.0)
        return;
    const auto kgH2O = nH2O * waterMolarMass;
    const auto kgH2Oinv = 1.0/kgH2O;
    for(auto i = 0; i < N; ++i)
    {
        J(i, i) = 1.0/n[i];
        J(i, iH2O) -= 1.0/nH2O;
    }
    J(iH2O, iH2O) = 0.0; // ensure sharp zero here as the above computation could produce round-off errors for this entry
}

/// Compute the Jacobian matrix of the species molalities in natural log (@eq{J=\frac{\partial\ln m}{\partial n}}).
/// @param n The vector with the species amounts.
/// @param iH2O The index of the water solvent species.
template<typename ArrayConstRef>
auto lnMolalitiesJacobian(ArrayConstRef&& n, Index iH2O)
{
    using T = Decay<decltype(n[0])>;
    const auto N = n.size();
    MatrixX<T> J(N, N);
    lnMolalitiesJacobian(n, iH2O, J);
    return J;
}

/// Compute the diagonal only of the Jacobian matrix of the species molalities in natural log (@eq{J=\frac{\partial\ln m}{\partial n}}).
/// @param n The vector with the species amounts.
/// @param iH2O The index of the water solvent species.
/// @param[out] D The output diagonal of the Jacobian matrix.
template<typename ArrayConstRef, typename VectorRef>
auto lnMolalitiesJacobianDiagonal(ArrayConstRef&& n, Index iH2O, VectorRef&& D) -> void
{
    //-----------------------------------------------------------------------------------------------------
    // == LATEX ==
    //-----------------------------------------------------------------------------------------------------
    // \frac{\partial\ln m_{i}}{\partial n_{i}}=\frac{1}{n_{i}}-\frac{\delta_{iw}}{n_{w}}
    //-----------------------------------------------------------------------------------------------------
    const auto N = n.size();
    assert(D.rows() == N);
    for(auto i = 0; i < N; ++i)
        D[i] = 1.0/n[i];
    D[iH2O] = 0.0;
}

/// Compute the diagonal only of the Jacobian matrix of the species molalities in natural log (@eq{J=\frac{\partial\ln m}{\partial n}}).
/// @param n The vector with the species amounts.
/// @param iH2O The index of the water solvent species.
template<typename ArrayConstRef>
auto lnMolalitiesJacobianDiagonal(ArrayConstRef&& n, Index iH2O)
{
    using T = Decay<decltype(n[0])>;
    const auto N = n.size();
    ArrayX<T> D(N);
    lnMolalitiesJacobianDiagonal(n, iH2O, D);
    return D;
}

} // namespace Reaktoro

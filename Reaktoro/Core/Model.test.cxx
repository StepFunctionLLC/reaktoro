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

// Catch includes
#include <catch2/catch.hpp>

// Reaktoro includes
#include <Reaktoro/Core/Model.hpp>
using namespace Reaktoro;

TEST_CASE("Testing Model class", "[Model]")
{
    Param K = 3.0;
    K.id("K");

    Vec<Param> params = { K };

    SECTION("Using ModelEvaluator")
    {
        auto evalfn = [=](real& res, real x, real y)
        {
            res = K*x*y;
        };

        Model<real(real, real)> model(evalfn, params);

        CHECK( model.initialized() );
        CHECK( model.evaluatorFn() );
        CHECK( model.calculatorFn() );

        CHECK( model.params().size() == 1 );
        CHECK( model.params()[0].id() == "K" );
        CHECK( model.params()[0].value() == 3.0 );

        const auto x = 3.0;
        const auto y = 7.0;

        CHECK( model(x, y) == Approx(3.0 * x * y) );

        K = 5.0;

        CHECK( model(x, y) == Approx(5.0 * x * y) );
    }

    SECTION("Using ModelCalculator")
    {
        auto calcfn = [=](real x, real y)
        {
            return K*x*y;
        };

        Model<real(real, real)> model(calcfn, params);

        CHECK( model.initialized() );
        CHECK( model.evaluatorFn() );
        CHECK( model.calculatorFn() );

        CHECK( model.params().size() == 1 );
        CHECK( model.params()[0].id() == "K" );
        CHECK( model.params()[0].value() == 3.0 );

        const auto x = 3.0;
        const auto y = 7.0;

        CHECK( model(x, y) == Approx(3.0 * x * y) );

        K = 5.0;

        CHECK( model(x, y) == Approx(5.0 * x * y) );
    }

    SECTION("Using ModelCalculator, Vec<Param>, and ModelSerializer")
    {
        Vec<Param> params = { Param("A", 1.0), Param("B", 2.0) };

        auto serializerfn = [=]()
        {
            Data data;
            data["A"] = params[0];
            data["B"] = params[1];
            return data;
        };

        auto calcfn = [=](real x, real y) -> real
        {
            auto A = params[0];
            auto B = params[1];
            return A*x + B*y;
        };

        auto model = Model<real(real, real)>(calcfn, params, serializerfn);

        CHECK( model.serialize().isDict() );

        Data data = model.serialize();

        CHECK( data["A"].asFloat() == 1.0 );
        CHECK( data["B"].asFloat() == 2.0 );
    }

    SECTION("Using Model::Constant")
    {
        auto model = Model<real(real, real)>::Constant(K);

        CHECK( model.initialized() );
        CHECK( model.evaluatorFn() );
        CHECK( model.calculatorFn() );

        CHECK( model.params().size() == 1 );
        CHECK( model.params()[0].id() == "K" );
        CHECK( model.params()[0].value() == 3.0 );

        const auto x = 3.0;
        const auto y = 7.0;

        CHECK( model(x, y) == Approx(3.0) );

        K = 5.0;

        CHECK( model(x, y) == Approx(5.0) );
    }
}

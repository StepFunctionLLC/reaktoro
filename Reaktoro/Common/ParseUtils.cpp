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

#include "ParseUtils.hpp"

// Reaktoro includes
#include <Reaktoro/Common/Algorithms.hpp>
#include <Reaktoro/Common/Exception.hpp>
#include <Reaktoro/Common/NamingUtils.hpp>
#include <Reaktoro/Common/StringUtils.hpp>

namespace Reaktoro {
namespace detail {

using StringIterator = String::iterator;

auto parseElementAtom(StringIterator begin, StringIterator end) -> Pair<String, StringIterator>
{
    if(begin == end) return {"", begin};
    auto endelement = find_if(begin + 1, end, [](char c){return isupper(c) || !isalpha(c);});
    String element = String(begin, endelement);
    if(islower(element[0])) return {"", endelement};
    return {element, endelement};
}

auto parseNumAtoms(StringIterator begin, StringIterator end) -> Pair<double, StringIterator>
{
    if(begin == end) return {1.0, begin};
    if(!(isdigit(*begin) || *begin == '.')) return {1.0, begin};
    auto endnumber = find_if(begin, end, [](char c){return !(isdigit(c) || c == '.');});
    double number = atof(String(begin, endnumber).c_str());
    return {number, endnumber};
}

auto findMatchedParenthesis(StringIterator begin, StringIterator end) -> StringIterator
{
    if(begin == end) return end;
    int level = 0;
    for(auto iter = begin+1; iter < end; ++iter)
    {
        level = (*iter == '(') ? level + 1 : level;
        level = (*iter == ')') ? level - 1 : level;
        if(*iter == ')' && level == -1)
            return iter;
    }
    return end;
}

auto findMatchedParenthesisReverse(StringIterator begin, StringIterator end) -> StringIterator
{
    if(begin == end) return begin;
    int level = 0;
    for(auto iter = end-2; iter != begin; --iter)
    {
        level = (*iter == ')') ? level + 1 : level;
        level = (*iter == '(') ? level - 1 : level;
        if(*iter == '(' && level == -1)
            return iter;
    }
    return begin;
}

auto parseChemicalFormulaAux(String const& formula, StringIterator begin, StringIterator end, Pairs<String, double>& result, double scalar) -> void
{
    if(begin == end) return;

    if(std::isdigit(*begin))  // CaCl2*10H2O, 2NaNO3*NH4NO3
    {
        auto res = parseNumAtoms(begin, end);

        const double number = res.first;

        StringIterator begin1 = res.second;
        StringIterator end1   = end;

        parseChemicalFormulaAux(formula, begin1, end1, result, scalar * number);
    }
    else if(std::isupper(*begin)) // H2O, HCO3-, CaCO3, Ab2Xyz3
    {
        const auto res1 = parseElementAtom(begin, end);
        const auto res2 = parseNumAtoms(res1.second, end);

        const auto element = res1.first;
        const auto natoms  = res2.first;

        const auto i = indexfn(result, RKT_LAMBDA(x, x.first == element));

        if(i < result.size()) // check if element symbol already exists in the result container
            result[i].second += scalar * natoms; // if so, increment coefficient
        else result.emplace_back(element, scalar * natoms); // otherwise, add into the container

        StringIterator begin1 = res2.second;
        StringIterator end1   = end;

        parseChemicalFormulaAux(formula, begin1, end1, result, scalar);
    }
    else if(*begin == '(')  // CaMg(CO3)2, (Ef(AbCd)3)2
    {
        StringIterator begin1 = begin + 1;
        StringIterator end1   = findMatchedParenthesis(begin, end);

        // Check if characters from begin1 to end1 are all lowercase letters,
        // indicating aggregate state symbols such as (aq), (l), (s), (cr), or
        // any other possible symbol, e.g., (xyz)
        bool alllowercase = true;
        for(auto iter = begin1; iter != end1; ++iter)
            alllowercase = alllowercase && std::islower(*iter);

        if(alllowercase) // check if the parenthesis contains only lowercase letters
            return; // ignore the rest of the formula string if aggregate state symbol such as (aq), (s), (cr), etc.

        auto res = parseNumAtoms(end1 + 1, end);

        const double number = res.first;

        StringIterator begin2 = res.second;
        StringIterator end2   = end;

        parseChemicalFormulaAux(formula, begin1, end1, result, scalar * number);
        parseChemicalFormulaAux(formula, begin2, end2, result, scalar);
    }
    else if(*begin == '*' || *begin == ':') // Symbols that are ignored, e.g., 2NaNO3*NH4NO3, 2NaNO3:NH4NO3
    {
        parseChemicalFormulaAux(formula, begin + 1, end, result, 1.0); // scalar is reset to 1.0 so that the formula following * or : does not use the current scalar, otherwise 2CaCl2*20H2O would be parsed with 40 atoms of O instead of 20!
    }
    else if(*begin == '+' || *begin == '-' || *begin == '[') // Ignore everything after these symbols
        return;
    else
    {
        errorif(*begin == ' ', "Error while parsing chemical formula: ", formula, ". Space characters are not allowed.");
        errorif(true, "Error while parsing chemical formula: ", formula, ". Found the invalid character: ", *begin);
    }
}

auto parseChemicalFormula(String formula) -> Pairs<String, double>
{
    // Parse the formula for elements and their coefficients (without charge)
    Pairs<String, double> result;

    // Check if formula represents charge species, which does not need to be parsed (no elements!)
    if(startswith(formula, "e-") || startswith(formula, "e[-]"))
        return result;

    // Recursively parse the formula (because of possible inner formulas in parenthesis)
    parseChemicalFormulaAux(formula, formula.begin(), formula.end(), result, 1.0);

    // Due to possible addition and multiplication operations, the number of
    // atoms of the elements may have some small round-off errors. For example,
    // for formula Na2SO4*(NH4)2SO4*4H2O, instead of 16.0 atoms of H, we may
    // endup with 15.99999998. The operation above cleans these round-off
    // errors.
    for(auto& pair : result)
        pair.second = (pair.second + 1e8) - 1e8;

    return result;
}

auto parseElectricChargeModeSignNumber(String formula) -> double
{
    size_t ipos = formula.find_last_of('+');
    size_t ineg = formula.find_last_of('-');
    size_t imin = std::min(ipos, ineg);

    if(imin == String::npos)
        return 0.0;

    int sign = (imin == ipos) ? +1 : -1;

    if(imin + 1 == formula.size())
        return sign;

    String digits = formula.substr(imin + 1);

    return sign * stod(digits);
}

auto parseElectricChargeModeMultipleSigns(String formula) -> double
{
    const auto sign = formula.back();
    const auto signval = sign == '+' ? 1 : (sign == '-' ? -1 : 0);
    const auto ilast = formula.size() - 1;
    if(sign != 0)
    {
        auto i = 0;
        while(formula[ilast - i] == sign && i <= ilast)
            ++i;
        const auto charge = i * signval;
        return charge;
    }
    else return 0;
}

auto parseElectricChargeModeNumberSignBetweenBrackets(String formula) -> double
{
    if(formula.back() != ']') return 0.0;

    size_t iparbegin = formula.rfind('[');

    if(iparbegin == String::npos) return 0.0;

    const auto isign = formula.size() - 2;
    const auto sign = formula[isign] == '+' ? +1.0 : formula[isign] == '-' ? -1.0 : 0.0;

    if(sign == 0.0) return 0.0;

    String digits(formula.begin() + iparbegin + 1, formula.end() - 2);

    if(digits.empty()) return sign;

    return sign * stod(digits);
}

auto parseElectricCharge(String formula) -> double
{
    const auto [formula0, suffix] = splitSpeciesNameSuffix(formula);

    double charge = {};
    charge = parseElectricChargeModeMultipleSigns(formula0); if(charge != 0.0) return charge;
    charge = parseElectricChargeModeNumberSignBetweenBrackets(formula0); if(charge != 0.0) return charge;
    charge = parseElectricChargeModeSignNumber(formula0); if(charge != 0.0) return charge;

    return 0.0;
}

} // namespace detail

auto parseReaction(const String& reaction) -> Pairs<String, double>
{
    Pairs<String, double> equation;
    auto words = split(reaction);
    for(const String& word : words)
    {
        Vec<String> pair = split(word, ":");
        equation.emplace_back(pair[1], tofloat(pair[0]));
    }
    return equation;
}

auto parseNumberStringPairs(const String& str) -> Pairs<String, double>
{
    auto words = split(str);
    Pairs<String, double> pairs;
    pairs.reserve(words.size());
    for(auto const& word : words)
    {
        const auto i = word.find(":");
        const auto coeff = tofloat(word.substr(0, i));
        const auto symbol = word.substr(i + 1);
        const auto j = indexfn(pairs, RKT_LAMBDA(x, x.first == symbol)); // check if symbol is already in pairs
        if(j < pairs.size())
            pairs[j].second += coeff; // if symbol alredy in pairs, increment coeff
        else pairs.push_back({symbol, coeff}); // otherwise, insert symbol and its coefficient
    }
    return pairs;
}

auto parseChemicalFormula(const String& formula) -> Pairs<String, double>
{
    return detail::parseChemicalFormula(formula);
}

auto parseElectricCharge(const String& formula) -> double
{
    return detail::parseElectricCharge(formula);
}

auto parseReactionEquation(const String& equation) -> Pairs<String, double>
{
    // Split the reaction equation into two words: reactants and products
    auto words = split(equation, "=");

    // Assert the equation has a single equal sign `=`
    errorif(words.size() > 2, "Cannot parse the reaction equation `" +  equation + "`. Expecting an equation with at most a single equal sign `=` separating " "reactants from products.");

    // The reactants and products as string (note below that if reaction does not contain =, then it only contains reactants, no products)
    const auto reactants_str = words[0];
    const auto products_str = words.size() == 2 ? words[1] : "";

    // Split the string representing the reactants and products at each `+` sign
    auto reactants = split(reactants_str, " ");
    auto products = split(products_str, " ");

    // The pairs of species names and stoichiometric coefficients
    Pairs<String, double> pairs;

    // Iterate over all strings representing pair number and species name in the reactants
    for(auto word : reactants)
    {
        if(word == "+") continue;
        auto pair = split(word, "*");
        auto number = pair.size() == 2 ? tofloat(pair[0]) : 1.0;
        auto species = pair.size() == 2 ? pair[1] : pair[0];
        pairs.emplace_back(species, -number); // negative sign for reactants
    }

    // Iterate over all strings representing pair number and species name in the products
    for(auto word : products)
    {
        if(word == "+") continue;
        auto pair = split(word, "*");
        auto number = pair.size() == 2 ? tofloat(pair[0]) : 1.0;
        auto species = pair.size() == 2 ? pair[1] : pair[0];
        pairs.emplace_back(species, number); // positive sign for products
    }

    return pairs;
}

} // namespace Reaktoro

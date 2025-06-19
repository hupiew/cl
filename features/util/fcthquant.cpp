/** fcthquant.cpp
   CL
   This file was originally written in Java as part of LIRE,
   and was adapted into C++ by Hupie.

   Copyright (C) 2002-2013 Mathias Lux (mathias@juggle.at)
     http://www.semanticmetadata.net/lire, http://www.lire-project.net
       Mathias Lux, Oge Marques. Visual Information Retrieval using Java and LIRE
       Morgan & Claypool, 2013
       URL: http://www.morganclaypool.com/doi/abs/10.2200/S00468ED1V01Y201301ICR025

   Copyright (C) 2022  Hupie (hupiew[at]gmail.com)

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <fcthquant.h>

#include <array>

namespace {

static constexpr int quant_len = 8 * 3; // 8 items * 8 tables
static constexpr double QuantTable[quant_len] = {
    0.0001300887781556944,
    0.00931731301788632,
    0.022434355689233364,
    0.04312054860272206,
    0.08316864016590504,
    0.1014305258997564,
    0.17484065838706805,
    0.22448041479670047,
    //};
    // static constexpr double QuantTable3[quant_len] = {
    0.000239769468748322,
    0.01732170431233569,
    0.039113643180734695,
    0.06933351209387438,
    0.07912246400035514,
    0.0909803325940354,
    0.16179593301552486,
    0.18472998648386427,
    //};
    // static constexpr double QuantTable7[quant_len] = {
    0.00018019686541079636,
    0.023730024499150865,
    0.04145715291254161,
    0.053918554375768424,
    0.06912246400035513,
    0.0819803325940354,
    0.09179593301552487,
    0.12472998648386426,
};
} // namespace

std::array<double, 192> FcthQuant::apply(
    const std::array<double, 192>& Local_Edge_Histogram)
{
    std::array<double, 192> Edge_HistogramElement{};

    for (int i = 0; i < 192; i++)
    {
        Edge_HistogramElement[i] = 0;
        // looks a bit weird but this selects the table we need,
        // at 0-47 it's table 0, 48-143 it's table 1, and 144-191 it's table 2
        const int quantOffset = (i / 48 - (i > 95)) * 8;

        double min = 1;
        for (int j = 0; j < 8; j++)
        {
            auto val = std::abs(Local_Edge_Histogram[i] - QuantTable[j + quantOffset]);
            if (val < min)
            {
                min = val;
                Edge_HistogramElement[i] = j;
            }
        }
    }
    return Edge_HistogramElement;
}

FcthQuant::FcthQuant() {}

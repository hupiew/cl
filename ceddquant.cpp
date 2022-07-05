// ceddquant.cpp
/* CL
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

#include <ceddquant.h>

#include <array>
#include <cstdlib>
#include <vector>


namespace {

constexpr double QuantTable[8 * 4] = {
    // Table 0
    0.00018019686541079636,
    0.023730024499150865,
    0.061457152912541606,
    0.11391855437576842,
    0.17912246400035514,
    0.2609803325940354,
    0.34179593301552486,
    0.5547299864838643,
    // Table 1
    0.00020925176965926233,
    0.022490587286241735,
    0.060250893514185,
    0.12070578805758059,
    0.18112808709063052,
    0.23413208135690056,
    0.32566061773310573,
    0.5207021758586574,
    // Table 2
    0.0004054642173212585,
    0.0048779763319071485,
    0.010882170090625908,
    0.018167239081219658,
    0.027043385568785293,
    0.03812941320129901,
    0.05267522131629386,
    0.07955540260700482,
    // Table 3
    0.0009688847597769558,
    0.01072515903365782,
    0.0241612053603767,
    0.041555917344385324,
    0.06289562844640226,
    0.09306627137969488,
    0.1369761331782207,
    0.26289786056221304,
};
} // namespace (anonymous)


std::vector<double> CEDDQuant::apply(const std::array<double, 144>& histogram)
{
    std::vector<double> Edge_HistogramElement(144);

    for (int i = 0; i < 144; i++)
    {
        Edge_HistogramElement[i] = 0;
        // selects the quant table, there are four of them we use the 1st for 0-23
        // 2nd for 24-47, 3rd for 48-95, and 4th for 96-143
        const int quantOffset = (i / 48 + (23 < i)) * 8;
        double min = 1;
        for (int j = 0; j < 8; j++)
        {
            const auto val = std::abs(histogram[i] - QuantTable[j + quantOffset]);
            if (val < min)
            {
                min = val;
                Edge_HistogramElement[i] = j;
            }
        }
    }
    return Edge_HistogramElement;
}

std::vector<double> CEDDQuant::compact_apply(const std::array<double, 144>& histogram)
{
    /*
     * In LIRE this is the size of the input array CEDD is 144,
     * the compact descriptor is supposed to be 60 doubles long.
     * But it seems to always be 144 long with every double past index 59 being 0.
     */
    std::vector<double> Edge_HistogramElement(60);

    for (int i = 0; i < 60; i++)
    {
        Edge_HistogramElement[i] = 0;
        /* selects table 1 for elements 0-9, table 2 for 10-19,
         * table 3 for 20-39 and table 4 for 40-59 */
        const int quantOffset = ((i / 20) + (9 < i)) * 8;
        double min = 1;
        for (int j = 0; j < 8; j++)
        {
            const auto val = std::abs(histogram[i] - QuantTable[j + quantOffset]);
            if (val < min)
            {
                min = val;

                Edge_HistogramElement[i] = j;
            }
        }
    }
    return Edge_HistogramElement;
}

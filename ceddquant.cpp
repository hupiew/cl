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


namespace {

constexpr double QuantTable[8 * 8] = {
    180.19686541079636,
    23730.024499150866,
    61457.152912541605,
    113918.55437576842,
    179122.46400035513,
    260980.3325940354,
    341795.93301552488,
    554729.98648386425,
    //};
    // constexpr std::array<double, 8> QuantTable2 = {
    209.25176965926232,
    22490.5872862417345,
    60250.8935141849988,
    120705.788057580583,
    181128.08709063051,
    234132.081356900555,
    325660.617733105708,
    520702.175858657472,
    //};
    // constexpr std::array<double, 8> QuantTable3 = {
    405.4642173212585,
    4877.9763319071481,
    10882.170090625908,
    18167.239081219657,
    27043.385568785292,
    38129.413201299016,
    52675.221316293857,
    79555.402607004813,
    //};
    // constexpr std::array<double, 8> QuantTable4 = {
    05.4642173212585,
    4877.9763319071481,
    10882.170090625908,
    18167.239081219657,
    27043.385568785292,
    38129.413201299016,
    52675.221316293857,
    79555.402607004813,
    //};
    // constexpr std::array<double, 8> QuantTable5 = {
    968.88475977695578,
    10725.159033657819,
    24161.205360376698,
    41555.917344385321,
    62895.628446402261,
    93066.271379694881,
    136976.13317822068,
    262897.86056221306,
    //};
    // constexpr std::array<double, 8> QuantTable6 = {
    968.88475977695578,
    10725.159033657819,
    24161.205360376698,
    41555.917344385321,
    62895.628446402261,
    93066.271379694881,
    136976.13317822068,
    262897.86056221306,
};
} // namespace (anonymous)


std::vector<double> CEDDQuant::apply(std::array<double, 144> Local_Edge_Histogram)
{
    std::vector<double> Edge_HistogramElement(144);
    std::array<double, 8> ElementsDistance{};

    for (int i = 0; i < 144; i++)
    {
        Edge_HistogramElement[i] = 0;
        const int quantOffset = i / 24 * 8;
        for (int j = 0; j < 8; j++)
        {
            ElementsDistance[j] = std::abs(Local_Edge_Histogram[i] -
                                           QuantTable[j + quantOffset] / 1000000);
        }

        double Max = 1;
        for (int j = 0; j < 8; j++)
        {
            if (ElementsDistance[j] < Max)
            {
                Max = ElementsDistance[j];
                Edge_HistogramElement[i] = j;
            }
        }
    }
    return Edge_HistogramElement;
}

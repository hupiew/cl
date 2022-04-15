// fcthquant.cpp
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

#include <fcthquant.h>

#include <array>

std::array<double, 192> FcthQuant::apply(
    const std::array<double, 192>& Local_Edge_Histogram)
{
    std::array<double, 192> Edge_HistogramElement{};
    std::array<double, 8> ElementsDistance{};

    for (int i = 0; i < 192; i++)
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

FcthQuant::FcthQuant() {}

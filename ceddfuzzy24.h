// ceddfuzzy24.h
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

#pragma once

#include <ceddfuzzy10.h>

class CeddFuzzy24
{
    static constexpr std::array<double, 8> SaturationMembershipValues =
        {0, 0, 68, 188, 68, 188, 255, 255};
    static constexpr std::array<double, 8> ValueMembershipValues =
        {0, 0, 68, 188, 68, 188, 255, 255};

    static constexpr int rules_length = 4;
    static constexpr FuzzyRules Fuzzy24BinRules[rules_length] =
    {
        {1, 1, 1},
        {0, 0, 2},
        {0, 1, 0},
        {1, 0, 2}
    };

    std::array<double, 3> resultsTable;
    std::array<double, 24> fuzzy24BinHisto;
    bool keep_values = false;

    void LOM_Defazzificator(const std::array<double, 2>& Input1,
                            const std::array<double, 2>& Input2);

    void MultiParticipate_Equal_Defazzificator(const std::array<double, 2>& Input1,
                                               const std::array<double, 2>& Input2);

    void MultiParticipate_Defazzificator(const std::array<double, 2>& Input1,
                                         const std::array<double, 2>& Input2);

public:
    CeddFuzzy24(bool keep_values);

    std::vector<double> ApplyFilter(double Hue,
                                    double Saturation,
                                    double Value,
                                    const std::vector<double>& ColorValues,
                                    int Method);

    static void FindMembershipValueForTriangles(const double Input,
                                                const std::array<double, 8>& Triangles,
                                                std::array<double, 2>* save_to);
};


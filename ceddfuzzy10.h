// ceddfuzzy10.h
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

#include <array>
#include <vector>


struct FuzzyRules
{
    int Input1;
    int Input2;
    int Input3;
    int Output;
};

class CeddFuzzy10
{
public:
    bool keep_values;
    std::array<double, 10> fuzzy10BinHisto;

    CeddFuzzy10(bool keep_values);

    // Method   0 = LOM
    //          1 = Multi Equal Participate
    //          2 = Multi Participate
    std::vector<double> apply_filter(double hue,
                                     double saturation,
                                     double value,
                                     int method);

    static void findMembershipValueForTriangles(const double input,
                                                const uint16_t* triangles,
                                                const int size,
                                                std::vector<double>* to_save);

    static void LOM_Defazzificator(const FuzzyRules Rules[],
                                   int rules_len,
                                   const std::vector<double>& Input1,
                                   const std::vector<double>& Input2,
                                   const std::vector<double>& Input3,
                                   std::array<double, 10>* ResultTable);

    static void MultiParticipate_Equal_Defazzificator(
        const FuzzyRules Rules[],
        int rules_len,
        const std::vector<double>& Input1,
        const std::vector<double>& Input2,
        const std::vector<double>& Input3,
        std::array<double, 10>* ResultTable);

    static void MultiParticipate_Defazzificator(const FuzzyRules Rules[],
                                                int rules_len,
                                                const std::vector<double>& Input1,
                                                const std::vector<double>& Input2,
                                                const std::vector<double>& Input3,
                                                std::array<double, 10>* ResultTable);

private:
    static constexpr uint16_t hueMembershipValues[32]
        {0, 0, 5, 10,
         5, 10, 35, 50,
         35, 50, 70, 85,
         70, 85, 150, 165,
         150, 165, 195, 205,
         195, 205, 265, 280,
         265, 280, 315, 330,
         315, 330, 360, 360};

    static constexpr uint16_t saturationMembershipValues[8]
        {0, 0, 10, 75,
         10, 75, 255, 255};

    static constexpr uint16_t valueMembershipValues[12]
        {0, 0, 10, 75,
         10, 75, 180, 220,
         180, 220, 255, 255};

    static constexpr int rules_length = 48;
    static constexpr FuzzyRules Fuzzy10BinRules[rules_length] =
    {
        {0, 0, 0, 2},
        {0, 1, 0, 2},
        {0, 0, 2, 0},
        {0, 0, 1, 1},
        {1, 0, 0, 2},
        {1, 1, 0, 2},
        {1, 0, 2, 0},
        {1, 0, 1, 1},
        {2, 0, 0, 2},
        {2, 1, 0, 2},
        {2, 0, 2, 0},
        {2, 0, 1, 1},
        {3, 0, 0, 2},
        {3, 1, 0, 2},
        {3, 0, 2, 0},
        {3, 0, 1, 1},
        {4, 0, 0, 2},
        {4, 1, 0, 2},
        {4, 0, 2, 0},
        {4, 0, 1, 1},
        {5, 0, 0, 2},
        {5, 1, 0, 2},
        {5, 0, 2, 0},
        {5, 0, 1, 1},
        {6, 0, 0, 2},
        {6, 1, 0, 2},
        {6, 0, 2, 0},
        {6, 0, 1, 1},
        {7, 0, 0, 2},
        {7, 1, 0, 2},
        {7, 0, 2, 0},
        {7, 0, 1, 1},
        {0, 1, 1, 3},
        {0, 1, 2, 3},
        {1, 1, 1, 4},
        {1, 1, 2, 4},
        {2, 1, 1, 5},
        {2, 1, 2, 5},
        {3, 1, 1, 6},
        {3, 1, 2, 6},
        {4, 1, 1, 7},
        {4, 1, 2, 7},
        {5, 1, 1, 8},
        {5, 1, 2, 8},
        {6, 1, 1, 9},
        {6, 1, 2, 9},
        {7, 1, 1, 3},
        {7, 1, 2, 3} };

};

// fuzzyfcth.cpp
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

#include <fuzzyfcth.h>

#include <ceddfuzzy10.h>
#include <fcth.h>

FuzzyFcth::FuzzyFcth() {}

void FuzzyFcth::apply_filter(const WaveletMatrixPlus& matrix,
                             const std::vector<double>& colors,
                             int method,
                             std::array<double, 192>* output)
{
    // Method   0 = LOM
    //          1 = Multi Equal Participate
    //          2 = Multi Participate

    resultsTable.fill(0.0);

    std::vector<double> HAct(2);
    std::vector<double> VAct(2);
    std::vector<double> EAct(2);

    CeddFuzzy10::findMembershipValueForTriangles(matrix.F1, HMemberValues, 8, &HAct);
    CeddFuzzy10::findMembershipValueForTriangles(matrix.F2, VMemberValues, 8, &VAct);
    CeddFuzzy10::findMembershipValueForTriangles(matrix.F3, EMemberValues, 8, &EAct);

    if (method == 0)
        CeddFuzzy10::LOM_Defazzificator(
            FcthRules, rules_length, HAct, VAct, EAct, &resultsTable);
    if (method == 1)
        CeddFuzzy10::MultiParticipate_Equal_Defazzificator(
            FcthRules, rules_length, HAct, VAct, EAct, &resultsTable);
    if (method == 2)
        CeddFuzzy10::MultiParticipate_Defazzificator(
            FcthRules, rules_length, HAct, VAct, EAct, &resultsTable);


    for (int i = 0; i < 8; i++)
    {
        if (resultsTable[i] > 0)
        {
            for (auto j = 0u; j < colors.size(); j++)
            {
                if (colors[j] > 0)
                    (*output)[colors.size() * i + j] += resultsTable[i] * colors[j];
            }
        }
    }
}

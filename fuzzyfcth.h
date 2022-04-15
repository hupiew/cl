// fuzzyfcth.h
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

#include <ceddfuzzy10.h>

struct WaveletMatrixPlus;

class FuzzyFcth
{
    // Only 8 are used, but it's 10 so it works with CeddFuzzy10 code
    std::array<double, 10> resultsTable;


    static constexpr uint16_t HMemberValues[] = { 0, 0, 20, 90, 20, 90, 255, 255 };
    static constexpr uint16_t VMemberValues[] = { 0, 0, 20, 90, 20, 90, 255, 255 };
    static constexpr uint16_t EMemberValues[] = { 0, 0, 20, 80, 20, 80, 255, 255 };

    static constexpr int rules_length = 8;
    static constexpr FuzzyRules FcthRules[rules_length] = {
        { 0, 0, 0, 0 }, { 0, 0, 1, 1 }, { 0, 1, 0, 2 }, { 0, 1, 1, 3 },
        { 1, 0, 0, 4 }, { 1, 0, 1, 5 }, { 1, 1, 0, 6 }, { 1, 1, 1, 7 }
    };

public:
    FuzzyFcth();

    void apply_filter(const WaveletMatrixPlus& matrix,
                      const std::vector<double>& colors,
                      int method,
                      std::array<double, 192>* output);
};

// fcth.h
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

#include <qimage.h>

struct WaveletMatrixPlus
{
    double F1;
    double F2;
    double F3;
    double Entropy;
};

class FCTH
{
    std::array<double, 192> histogram;
    bool compact;

    WaveletMatrixPlus singlePassThreshold(const std::array<double, 16>& inputMatrix,
                                          int level) const noexcept;

public:
    FCTH();

    void extract(const QImage& image);

    std::vector<int8_t> get_descriptor() const noexcept;

    const std::array<double, 192>& get_data() const noexcept;
};
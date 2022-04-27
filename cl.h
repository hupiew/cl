// cl.h (ColorLayout)
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

#include <QImage>

#include <extractor.h>


class ColorLayoutExtractor : public Extractor
{
    std::array<std::array<int, 64>, 3> shapes;
    std::array<std::array<uint8_t, 64>, 3> coeffs;
    unsigned int ycoeff_length{ 21 };
    unsigned int ccoeff_length{ 6 };

    static int quant_ac (int i) noexcept;
    static int quant_cdc(int i) noexcept;
    static int quant_ydc(int i) noexcept;

    void fdct(std::array<int, 64> &shape) noexcept;
    void extract_shape(const QImage &image) noexcept;

public:
    ColorLayoutExtractor();

    virtual void extract(const QImage& image) noexcept override;

    [[nodiscard]] virtual std::vector<int8_t> get_descriptor() const override;

    virtual bool is_variable() const noexcept override { return false; }

    void set_coeffs(unsigned int ycoeffs, unsigned int ccoeffs);
};

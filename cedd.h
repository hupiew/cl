/** cedd.h
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

#pragma once

#include <array>

#include <QImage>

#include <extractor.h>


struct Neighborhood
{
    double Area1;
    double Area2;
    double Area3;
    double Area4;
};

class Area
{
    uint32_t red;
    uint32_t green;
    uint32_t blue;

public:
    void add_color(const QRgb& col) noexcept
    {
        red += qRed(col);
        green += qGreen(col);
        blue += qBlue(col);
    }
    uint32_t get_blue() const noexcept { return blue; }
    uint32_t get_green() const noexcept { return green; }
    uint32_t get_red() const noexcept { return red; }
    double to_gray() const noexcept
    {
        return 0.114 * blue + 0.587 * green + 0.299 * red;
    }
};


struct MaskResults
{
    MaskResults(const Neighborhood& init);

    double max() const noexcept;

    double Mask1;
    double Mask2;
    double Mask3;
    double Mask4;
    double Mask5;
};

class CEDD : public Extractor
{
    static constexpr float T0 = 14.0f;
    static constexpr float T1 = 0.68f;
    static constexpr float T2 = 0.98f;
    static constexpr float T3 = 0.98f;

    std::array<int8_t, 144> histogram;
    bool compact;

public:
    CEDD();

    virtual void extract(const QImage& image) override;

    virtual std::vector<int8_t> get_descriptor() const override;

    const std::array<int8_t, 144>& get_data() const noexcept;

    static size_t index_2d_arrayC(size_t x, size_t y, size_t width)
    {
        // C-style ordering
        return y * width + x;
    }
};

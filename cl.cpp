/** cl.cpp
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

#include <cl.h>

#include <algorithm>
#include <array>
#include <iterator>
#include <stdexcept>


constexpr double cosins[8][8] = {
        {
            3.535534e-01,
            3.535534e-01,
            3.535534e-01,
            3.535534e-01,
            3.535534e-01,
            3.535534e-01,
            3.535534e-01,
            3.535534e-01,
        },
        {
            4.903926e-01,
            4.157348e-01,
            2.777851e-01,
            9.754516e-02,
            -9.754516e-02,
            -2.777851e-01,
            -4.157348e-01,
            -4.903926e-01,
        },
        {
            4.619398e-01,
            1.913417e-01,
            -1.913417e-01,
            -4.619398e-01,
            -4.619398e-01,
            -1.913417e-01,
            1.913417e-01,
            4.619398e-01,
        },
        {
            4.157348e-01,
            -9.754516e-02,
            -4.903926e-01,
            -2.777851e-01,
            2.777851e-01,
            4.903926e-01,
            9.754516e-02,
            -4.157348e-01,
        },
        {
            3.535534e-01,
            -3.535534e-01,
            -3.535534e-01,
            3.535534e-01,
            3.535534e-01,
            -3.535534e-01,
            -3.535534e-01,
            3.535534e-01,
        },
        {
            2.777851e-01,
            -4.903926e-01,
            9.754516e-02,
            4.157348e-01,
            -4.157348e-01,
            -9.754516e-02,
            4.903926e-01,
            -2.777851e-01,
        },
        {
            1.913417e-01,
            -4.619398e-01,
            4.619398e-01,
            -1.913417e-01,
            -1.913417e-01,
            4.619398e-01,
            -4.619398e-01,
            1.913417e-01,
        },
        {
            9.754516e-02,
            -2.777851e-01,
            4.157348e-01,
            -4.903926e-01,
            4.903926e-01,
            -4.157348e-01,
            2.777851e-01,
            -9.754516e-02,
        }
};

constexpr uint8_t arrayZigZag[] ={
    0 , 1 , 8 , 16, 9 , 2 , 3 , 10,
    17, 24, 32, 25, 18, 11, 4 , 5 ,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6 , 7 , 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
   };

ColorLayoutExtractor::ColorLayoutExtractor() :
    shapes(), coeffs()
{

}

void ColorLayoutExtractor::extract(const QImage& image) noexcept
{
    extract_shape(image);
    for (auto& shape: shapes)
        fdct(shape);

    coeffs[0][0] = static_cast<uint8_t>(quant_ydc(shapes[0][0] >> 3) >> 1);
    coeffs[1][0] = static_cast<uint8_t>(quant_cdc(shapes[1][0] >> 3));
    coeffs[2][0] = static_cast<uint8_t>(quant_cdc(shapes[2][0] >> 3));

    for (auto i = 1u; i < coeffs[0].size(); ++i)
    {
        coeffs[0][i] = static_cast<uint8_t>(quant_ac((shapes[0][(arrayZigZag[i])]) >> 1) >> 3);
        coeffs[1][i] = static_cast<uint8_t>(quant_ac(shapes[1][(arrayZigZag[i])]) >> 3);
        coeffs[2][i] = static_cast<uint8_t>(quant_ac(shapes[2][(arrayZigZag[i])]) >> 3);
    }

}

std::vector<int8_t> ColorLayoutExtractor::get_descriptor() const
{
    std::vector<int8_t> desc{};

    desc.reserve(ycoeff_length + ccoeff_length * 2);

    std::copy_n(coeffs[0].begin(), ycoeff_length, std::back_inserter(desc));
    std::copy_n(coeffs[1].begin(), ccoeff_length, std::back_inserter(desc));
    std::copy_n(coeffs[2].begin(), ccoeff_length, std::back_inserter(desc));

    return desc;
}

void ColorLayoutExtractor::set_coeffs(unsigned int ycoeffs, unsigned int ccoeffs)
{
    if (64 < ycoeffs || ycoeffs == 0 || 64 < ccoeffs || ccoeffs == 0)
        throw std::runtime_error("ycoeff and ccoeff need to be positive non-zero "
                                 "numbers less-than or equal to 64");
    ycoeff_length = ycoeffs;
    ccoeff_length = ccoeffs;
}

int ColorLayoutExtractor::quant_ac(int i) noexcept
{
    using std::abs;

    int j = 0;
    i = std::clamp(i, -256, 255);

    const bool b = abs(i) > 127;
    const bool b2 = abs(i) > 63;

    j = abs((32 * (b + b2)) + (abs(i) >> (b + b2)));

    j = i < 0 ? -j : j;

    j += 128;

    return j; //# largest value returned is 255
}

int ColorLayoutExtractor::quant_cdc(int i) noexcept
{
    int j = 0;
    if      (i > 160)  j = (i >> 2) + 16;
    else if (i > 144)  j = (i >> 1) - 24;
    else if (i > 112)  j = i - 96;
    else if (i >  96)  j = (i >> 1) - 40;
    else               j = (i >> 2) - 16;
    j = std::min(std::max(j, 0), 63);

    return j;
}

int ColorLayoutExtractor::quant_ydc(int i) noexcept
{
    int j = 0;
    if      (i > 192)  j = (i >> 2) + 64;
    else if (i > 160)  j = (i >> 1) + 16;
    else if (i >  96)  j = i - 64;
    else if (i >  64)  j = (i >> 1) - 16;
    else               j = i >> 2;

    return j;
}

void ColorLayoutExtractor::fdct(std::array<int, 64> &shape) noexcept
{
   std::array<double, 64> dct{};

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            double s { 0.0 };
            for (int k = 0; k < 8; k++) s += cosins[j][k] * shape[8 * i + k];
            dct[8 * i + j] = s; // 0-63
        }
    }

    for (int j = 0; j < 8; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            double s{ 0.0 };
            for (int k = 0; k < 8; k++)
                s += cosins[i][k] * dct[8 * k + j];
            shape[8 * i + j] = static_cast<int>(s + 0.499999);
        }
    }
}

void ColorLayoutExtractor::extract_shape(const QImage& image) noexcept
{
    std::array<int, 64> cnt{};
    std::array<std::array<int, 64>, 3> sum{};

    for (int y = 0; y < image.height(); ++y)
    {
        const QRgb *line = reinterpret_cast<const QRgb*>(image.scanLine(y));
        const int y_axis = y * 8 / image.height();
        for (int x = 0; x < image.width(); ++x)
        {
            const QRgb &rgb = line[x];
            const int red = qRed(rgb);
            const int green = qGreen(rgb);
            const int blue = qBlue(rgb);

            const int x_axis = x * 8 / image.width();
            const int k = y_axis * 8 + x_axis;

            const double yy = (0.299 * red + 0.587 * green + 0.114 * blue) / 256.0;
            sum[0][k] += static_cast<int>(219.0 * yy + 16.5);
            sum[1][k] += static_cast<int>(224.0 * 0.564 * (blue / 256.0 - yy) + 128.5);
            sum[2][k] += static_cast<int>(224.0 * 0.713 * (red  / 256.0 - yy) + 128.5);
            ++cnt[k];
        }
    }

    for (int k = 0; k < 3; ++k)
    {
        for (int i = 0; i < 64; ++i)
        {
            shapes[k][i] = (cnt[i] != 0) ? (sum[k][i] / cnt[i]) : 0;
        }
    }
}

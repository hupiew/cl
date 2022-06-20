// phash.cpp
/* CL
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

#include <phash.h>

#include <cmath>
#include <stdexcept>

#include <cedd.h>


PHash::PHash()
  : hash(0)
{}

void PHash::extract(const QImage& image)
{
    // We do a faster rougher transform first because it's saves a lot of time.
    // Compared to scaling straight to 32x32 it's a 50x speedup, on my comp at least.
    const auto interm = image.scaledToHeight(180, Qt::FastTransformation);
    const auto resized =
        interm.scaled(SIDE, SIDE, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    const auto dct = dct_cv(resized);

    float sum = 0;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            const auto index = CEDD::index_2d_arrayC(j, i, SIDE);
            sum += dct[index];
        }
    }
    sum -= dct[0];
    const auto mean = sum / 64; // 8*8

    uint64_t hash = 0;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            const auto index = CEDD::index_2d_arrayC(j, i, SIDE);

            hash = hash << 1;
            if (dct[index] < mean) hash += 1;
        }
    }
    this->hash = hash;
}

std::vector<int8_t> PHash::get_descriptor() const
{
    constexpr auto bytesInHash = 8;
    std::vector<int8_t> hashdata(bytesInHash);

    auto ptr = reinterpret_cast<uint64_t*>(hashdata.data());
    *ptr = hash;

    return hashdata;
}

std::vector<float> PHash::dct_cv(const QImage& image)
{
    if (image.width() != image.height() || image.width() != SIDE)
        throw std::length_error("Image has the wrong dimentions.");
    auto output = std::vector<float>(SIDE * SIDE);

    for (int i = 0; i < image.height(); ++i)
    {
        const QRgb* line = reinterpret_cast<const QRgb*>(image.scanLine(i));
        for (int j = 0; j < image.width(); ++j)
        {
            const QRgb& col = line[j];

            const auto red = qRed(col);
            const auto green = qGreen(col);
            const auto blue = qBlue(col);
            const auto gray = 0.114 * blue + 0.587 * green + 0.299 * red;

            const auto index = CEDD::index_2d_arrayC(j, i, image.width());
            output[index] = gray;
        }
    }

    // Row dct
    for (int i = 0; i < SIDE; ++i)
    {
        dct_step(&output, i * SIDE, 1);
    }

    // Column dct
    for (int i = 0; i < SIDE; ++i)
    {
        dct_step(&output, i, SIDE);
    }

    dct_scale(&output);

    return output;
}

void PHash::dct_step(std::vector<float>* in_vector, int start, int step)
{
    std::array<float, SIDE> tmp;
    constexpr float piN = (355.f / 113) / SIDE;
    auto& input = *in_vector;

    for (int k = 0; k < SIDE; ++k)
    {
        float cnt = 0.0;
        for (int n = 0; n < SIDE; ++n)
        {
            const auto xn = input[start + step * n];
            cnt += xn * std::cos(piN * (n + 0.5) * k);
        }
        tmp[k] = cnt;
    }

    for (int k = 0; k < SIDE; ++k)
    {
        const auto index = start + step * k;
        input[index] = tmp[k];
    }
}

void PHash::dct_scale(std::vector<float>* input)
{
    const auto f = std::sqrt(1 / (32.f * 2));
    const auto fe = std::sqrt(1 / (32.f * 4));

    (*input)[0] = (*input)[0] * f;
    for (int i = 0; i < SIDE; ++i)
    {
        for (int j = 0; j < SIDE; ++j)
        {
            const auto index = CEDD::index_2d_arrayC(j, i, SIDE);
            if (i == 0 && j == 0) continue;
            if (i == 0 || j == 0) (*input)[index] = (*input)[index] * fe;
            else
                (*input)[index] = (*input)[index] * f;
        }
    }
}

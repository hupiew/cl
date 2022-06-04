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
        interm.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    const auto dct = dct_cv(resized);

    float sum = 0;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            const auto index = CEDD::index_2d_arrayC(j, i, 32);
            sum += dct[index];
        }
    }
    sum -= dct[0];
    const auto mean = sum / 64;

    uint64_t hash = 0;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            const auto index = CEDD::index_2d_arrayC(j, i, 32);

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
    if (image.width() != image.height() && image.width() != 32)
        throw std::length_error("Image has the wrong dimentions.");
    auto output = std::vector<float>(32 * 32);

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
    for (int i = 0; i < 32; ++i)
    {
        dct_step(&output, i * 32, 1);
    }

    // Column dct
    for (int i = 0; i < 32; ++i)
    {
        dct_step(&output, i, 32);
    }

    dct_scale(&output);

    return output;
}

void PHash::dct_step(std::vector<float>* in_vector, int start, int step)
{
    std::array<float, 32> tmp;
    constexpr int N = 32;
    constexpr float piN = (355.f / 113) / N;
    auto& input = *in_vector;

    for (int k = 0; k < N; ++k)
    {
        float cnt = 0.0;
        for (int n = 0; n < 32; ++n)
        {
            const auto xn = input[start + step * n];
            cnt += xn * std::cos(piN * (n + 0.5) * k);
        }
        tmp[k] = cnt;
    }

    for (int k = 0; k < N; ++k)
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
    for (int i = 0; i < 32; ++i)
    {
        for (int j = 0; j < 32; ++j)
        {
            const auto index = CEDD::index_2d_arrayC(j, i, 32);
            if (i == 0 && j == 0) continue;
            if (i == 0 || j == 0) (*input)[index] = (*input)[index] * fe;
            else
                (*input)[index] = (*input)[index] * f;
        }
    }
}

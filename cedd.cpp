// cedd.cpp
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

#include <cedd.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include <ceddfuzzy10.h>
#include <ceddfuzzy24.h>
#include <ceddquant.h>
#include <compactceddquant.h>

CEDD::CEDD() : compact(false)
{

}

void CEDD::extract(const QImage& image)
{
    if (image.format() != QImage::Format_RGB32)
    {
        std::cout << "image format not correct" << '\n';
    }

    std::array<double, 144> CEDD{};

    const int width = image.width();
    const int height = image.height();

    auto ImageGrid = std::vector<double>(height * width);
    auto ImageGridRed = std::vector<int>(height * width);
    auto ImageGridGreen = std::vector<int>(height * width);
    auto ImageGridBlue = std::vector<int>(height * width);


//please double check from here
    int NumberOfBlocks = -1;

    if (std::min(width, height) >= 80) NumberOfBlocks = 1600;
    if (std::min(width, height) < 80 && std::min(width, height) >= 40) NumberOfBlocks = 400;
    if (std::min(width, height) < 40) NumberOfBlocks = -1;

    int Step_X = 2;
    int Step_Y = 2;

    if (NumberOfBlocks > 0)
    {
        Step_X = static_cast<int>(std::floor(width / std::sqrt(NumberOfBlocks)));
        Step_Y = static_cast<int>(std::floor(height / std::sqrt(NumberOfBlocks)));

        if ((Step_X % 2) != 0)
        {
            Step_X = Step_X - 1;
        }
        if ((Step_Y % 2) != 0)
        {
            Step_Y = Step_Y - 1;
        }
    }

// to here

    CEDD.fill(0.0);

    for (int y = 0; y < height; y++)
    {
        const QRgb* line = reinterpret_cast<const QRgb*>(image.scanLine(y));
        for (int x = 0; x < width; x++)
        {
            const QRgb &rgb = line[x];
            const int r = qRed(rgb);
            const int g = qGreen(rgb);
            const int b = qBlue(rgb);
            // this
            const auto index = index_2d_arrayC(x, y, width);

            ImageGridRed[index] = r;
            ImageGridGreen[index] = g;
            ImageGridBlue[index] = b;

            ImageGrid[index] = (0.114 * b + 0.587 * g + 0.299 * r);
        }
    }

//plase double check from here

    int TemoMAX_X = Step_X * std::floor(image.width() >> 1);
    int TemoMAX_Y = Step_Y * std::floor(image.height() >> 1);

    if (NumberOfBlocks > 0) {
        TemoMAX_X = Step_X * std::sqrt(NumberOfBlocks);
        TemoMAX_Y = Step_Y * std::sqrt(NumberOfBlocks);
    }


//to here

    CeddFuzzy10 Fuzzy10{ false };
    CeddFuzzy24 Fuzzy24{ false };
    for (int y = 0; y < TemoMAX_Y; y += Step_Y)
    {
        for (int x = 0; x < TemoMAX_X; x += Step_X)
        {
            int MeanRed = 0;
            int MeanGreen = 0;
            int MeanBlue = 0;
            Neighborhood PixelsNeighborhood{};
            std::array<int, 6> Edges{};
            Edges.fill(-1);

            // TEST HERE
            for (int i = y; i < y + Step_Y; i++)
            {
                for (int j = x; j < x + Step_X; j++)
                {
                    const auto index = index_2d_arrayC(j, i, width);
                    MeanRed   += ImageGridRed[index];
                    MeanGreen += ImageGridGreen[index];
                    MeanBlue  += ImageGridBlue[index];

                    const auto pixel = ImageGrid[index];

                    if (j < (x + Step_X / 2) && i < (y + Step_Y / 2))
                        PixelsNeighborhood.Area1 += (pixel);
                    if (j >= (x + Step_X / 2) && i < (y + Step_Y / 2))
                        PixelsNeighborhood.Area2 += (pixel);
                    if (j < (x + Step_X / 2) && i >= (y + Step_Y / 2))
                        PixelsNeighborhood.Area3 += (pixel);
                    if (j >= (x + Step_X / 2) && i >= (y + Step_Y / 2))
                        PixelsNeighborhood.Area4 += (pixel);

                }
            }
            const double dd = 4.0 / (Step_X * Step_Y);
            PixelsNeighborhood.Area1 = static_cast<int>(PixelsNeighborhood.Area1 * dd);
            PixelsNeighborhood.Area2 = static_cast<int>(PixelsNeighborhood.Area2 * dd);
            PixelsNeighborhood.Area3 = static_cast<int>(PixelsNeighborhood.Area3 * dd);
            PixelsNeighborhood.Area4 = static_cast<int>(PixelsNeighborhood.Area4 * dd);

            MaskResults MaskValues{ PixelsNeighborhood };

            const double Max = MaskValues.max();

            MaskValues.Mask1 = MaskValues.Mask1 / Max;
            MaskValues.Mask2 = MaskValues.Mask2 / Max;
            MaskValues.Mask3 = MaskValues.Mask3 / Max;
            MaskValues.Mask4 = MaskValues.Mask4 / Max;
            MaskValues.Mask5 = MaskValues.Mask5 / Max;

            int T = -1;

            if (Max < T0) {
                Edges[0] = 0;
                T = 0;
            } else {
                T = -1;

                if (MaskValues.Mask1 > T1) {
                    T++;
                    Edges[T] = 1;
                }
                if (MaskValues.Mask2 > T2) {
                    T++;
                    Edges[T] = 2;
                }
                if (MaskValues.Mask3 > T2) {
                    T++;
                    Edges[T] = 3;
                }
                if (MaskValues.Mask4 > T3) {
                    T++;
                    Edges[T] = 4;
                }
                if (MaskValues.Mask5 > T3) {
                    T++;
                    Edges[T] = 5;
                }

            }

            MeanRed   = MeanRed   / (Step_Y * Step_X);
            MeanGreen = MeanGreen / (Step_Y * Step_X);
            MeanBlue  = MeanBlue  / (Step_Y * Step_X);

            const QColor col = { MeanRed, MeanGreen, MeanBlue };
            // Qt returns a hue value of -1 for achromatic colors, not what we want.
            const auto hue = std::max(0, static_cast<int>(col.hsvHueF() * 359));
            const auto sat = static_cast<int>(col.hsvSaturationF() * 256);
            const auto val = static_cast<int>(col.valueF() * 256);

            if (!this->compact)
            {
                auto fuzzy10 = Fuzzy10.apply_filter(hue, sat, val, 2);
                auto fuzzy24 = Fuzzy24.ApplyFilter(sat, val, fuzzy10, 2);

                for (int i = 0; i <= T; i++)
                {
                    for (int j = 0; j < 24; j++)
                    {
                        if (fuzzy24[j] > 0)
                            CEDD[24 * Edges[i] + j] += fuzzy24[j];
                    }
                }
            }
            else
            {
                auto fuzzy10_table = Fuzzy10.apply_filter(hue, sat, val,2);
                for (int i = 0; i <= T; i++)
                {
                    for (int j = 0; j < 10; j++)
                    {
                        if (fuzzy10_table[j] > 0)
                            CEDD[10 * Edges[i] + j] += fuzzy10_table[j];
                    }
                }
            }
        }
    }
    const double sum = std::accumulate(CEDD.cbegin(), CEDD.cend(), 0.0);

    for (int i = 0; i < 144; i++)
    {
        CEDD[i] = CEDD[i] / sum;
    }

    std::vector<double> qCEDD;


    if (!this->compact)
    {
        qCEDD = CEDDQuant::apply(CEDD);
    }
    else
    {
        qCEDD = CompactCEDDQuant::apply(CEDD);
    }

    for (auto i = 0u; i < qCEDD.size(); i++)
    {
        histogram[i] = static_cast<int8_t>(qCEDD[i]);
    }
}

std::vector<int8_t> CEDD::get_descriptor()
{
    int position = -1;
    for (auto i = 0u; i < histogram.size(); i++)
    {
        if (position == -1)
        {
            if (histogram[i] == 0)  position = i;
        }
        else if (position > -1)
        {
            if (histogram[i] != 0)  position = -1;
        }
    }
    if (position < 0) position = 143;
    // find out the actual length. two values in one byte, so we have to round up.
    int length = (position + 1) / 2;
    if ((position + 1) % 2 == 1) length = position / 2 + 1;
    std::vector<int8_t> result(length);
    for (auto i = 0u; i < result.size(); i++)
    {
        int tmp = (histogram[(i << 1)]) << 4;
        tmp = (tmp | (histogram[(i << 1) + 1]));
        result[i] = static_cast<uint8_t>(tmp - 128);
    }
    return result;
}

const std::array<int8_t, 144>& CEDD::get_data() const noexcept { return histogram; }

MaskResults::MaskResults(const Neighborhood &init)
{
    using std::abs, std::sqrt;
    Mask1 = abs(init.Area1 * 2 + init.Area2 * -2 + init.Area3 * -2 + init.Area4 * 2);
    Mask2 = abs(init.Area1 * 1 + init.Area2 * 1 + init.Area3 * -1 + init.Area4 * -1);
    Mask3 = abs(init.Area1 * 1 + init.Area2 * -1 + init.Area3 * 1 + init.Area4 * -1);
    Mask4 = abs(init.Area1 * sqrt(2) + init.Area4 * -sqrt(2));
    Mask5 = abs(init.Area2 * sqrt(2) + init.Area3 * -sqrt(2));
}

double MaskResults::max() const noexcept
{
    using std::max;
    auto tmp1 = max(Mask1, Mask2);
    auto tmp2 = max(Mask3, Mask4);
    auto tmp3 = max(tmp1, tmp2);

    return max(tmp3, Mask5);
}

// fcth.cpp
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

#include <fcth.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>

// The CEDD fuzzy code is identical to the FCTH code.
#include <cedd.h>
#include <ceddfuzzy10.h>
#include <ceddfuzzy24.h>
#include <fcthquant.h>
#include <fuzzyfcth.h>


FCTH::FCTH()
  : histogram()
  , compact(false)
{

}

void FCTH::extract(const QImage &image)
{
    auto Fuzzy10 = CeddFuzzy10(false);
    auto Fuzzy24 = CeddFuzzy24(false);
    auto fuzzFcth = FuzzyFcth();

    const int Method = 2;
    const int width = image.width();
    const int height = image.height();

    histogram.fill(0.0);

    const int NumberOfBlocks = 1600;
    int Step_X = static_cast<int>(std::floor(width / std::sqrt(NumberOfBlocks)));
    int Step_Y = static_cast<int>(std::floor(height / std::sqrt(NumberOfBlocks)));

    if ((Step_X % 2) != 0) {
        Step_X = Step_X - 1;
    }
    if ((Step_Y % 2) != 0) {
        Step_Y = Step_Y - 1;
    }


    if (Step_Y < 4) Step_Y = 4;
    if (Step_X < 4) Step_X = 4;
    ///
    // Filter

    for (int y = 0; y < height - Step_Y; y += Step_Y)
    {
        for (int x = 0; x < width - Step_X; x += Step_X)
        {
            std::array<double, 16> Block{};
            std::array<int, 16> BlockCount{};

            int MeanRed = 0;
            int MeanGreen = 0;
            int MeanBlue = 0;

            //#endregion

            int TempSum = 0;

            for (int j = 0; j < Step_Y; j++)
            {
                const QRgb* line = reinterpret_cast<const QRgb*>(image.scanLine(y + j));
                for (int i = 0; i < Step_X; i++)
                {
                    int CurrentPixelX = 3;
                    int CurrentPixelY = 3;
                    CurrentPixelX -= (i < (Step_X / 4));
                    CurrentPixelX -= (i < (Step_X / 2));
                    CurrentPixelX -= (i < (3 * Step_X / 4));

                    CurrentPixelY -= (j < (Step_Y / 4));
                    CurrentPixelY -= (j < (Step_Y / 2));
                    CurrentPixelY -= (j < (3 * Step_Y / 4));

                    const QRgb& rgb = line[x + i];
                    const int r = qRed(rgb);
                    const int g = qGreen(rgb);
                    const int b = qBlue(rgb);
                    const double gray = (0.114 * b + 0.587 * g + 0.299 * r);

                    const auto block_index =
                        CEDD::index_2d_arrayC(CurrentPixelX, CurrentPixelY, 4);

                    Block[block_index] += gray;
                    BlockCount[block_index]++;

                    MeanRed += r;
                    MeanGreen += g;
                    MeanBlue += b;

                    TempSum++;
                }
            }


            for (int i = 0; i < BlockCount.size(); i++)
            {
                Block[i] = Block[i] / BlockCount[i];
            }
            const auto Matrix = singlePassThreshold(Block, 1);

            MeanRed = static_cast<int>(MeanRed / (Step_Y * Step_X));
            MeanGreen = static_cast<int>(MeanGreen / (Step_Y * Step_X));
            MeanBlue = static_cast<int>(MeanBlue / (Step_Y * Step_X));

            const QColor col = { MeanRed, MeanGreen, MeanBlue };
            // Qt returns a hue value of -1 for achromatic colors, not what we want.
            const auto hue = std::max(0.0f, static_cast<float>(col.hsvHueF() * 359));
            const auto sat = col.hsvSaturationF() * 255;
            const auto val = col.valueF() * 255;

            if (compact == false)
            {
                auto Fuzzy10BinResultTable =
                    Fuzzy10.apply_filter(hue, sat, val, Method);
                auto Fuzzy24BinResultTable =
                    Fuzzy24.ApplyFilter(sat, val, Fuzzy10BinResultTable, Method);
                fuzzFcth.apply_filter(
                    Matrix, Fuzzy24BinResultTable, Method, &histogram);
                // possible problem
            }
            else
            {
                auto Fuzzy10BinResultTable =
                    Fuzzy10.apply_filter(hue, sat, val, Method);
                fuzzFcth.apply_filter(
                    Matrix, Fuzzy10BinResultTable, Method, &histogram);
            }
        }
    }

    const double sum = std::accumulate(histogram.cbegin(), histogram.cend(), 0.0);

    for (auto i = 0u; i < histogram.size(); i++)
    {
        histogram[i] = histogram[i] / sum;
    }

    histogram = FcthQuant::apply(histogram);
}

std::vector<int8_t> FCTH::get_descriptor() const
{
    int position = -1;
    for (auto i = 0u; i < histogram.size(); i++)
    {
        if (position == -1)
        {
            if (histogram[i] == 0) position = i;
        }
        else if (position > -1)
        {
            if (histogram[i] != 0) position = -1;
        }
    }
    if (position < 0) position = static_cast<int>(histogram.size() - 1);
    // find out the actual length. two values in one byte, so we have to round up.
    int length = (position + 1) / 2;
    if ((position + 1) % 2 == 1) length = position / 2 + 1;
    std::vector<int8_t> result(length);
    for (auto i = 0u; i < result.size(); i++)
    {
        int tmp = (static_cast<int>(histogram[(i << 1)] * 2)) << 4;
        tmp = (tmp | ((int)(histogram[(i << 1) + 1] * 2)));
        result[i] = static_cast<int8_t>(tmp - 128);
    }
    return result;
}

const std::array<double, 192>& FCTH::get_data() const noexcept { return histogram; }

WaveletMatrixPlus FCTH::singlePassThreshold(const std::array<double, 16>& inputMatrix,
                                            int level) const noexcept
{
    // inputMatrix is a flattened square 2d array, this is the length of a side.
    constexpr int side_len = 4;
    WaveletMatrixPlus TempMatrix;
    level = static_cast<int>(std::pow(2, level - 1));

    std::array<double, 16> resultMatrix{};

    const int xOffset = 4 / (2 * level);

    const int yOffset = 4 / (2 * level);

    int currentPixel = 0;
    constexpr double multiplier = 0;


    for (int y = 0; y < side_len; y++)
    {
        for (int x = 0; x < side_len; x++)
        {
            if ((y < side_len / (2 * level)) && (x < side_len / (2 * level)))
            {
                currentPixel++;

                const auto index = CEDD::index_2d_arrayC(x, y, side_len);
                const auto xx = x * 2;
                const auto yy = y * 2;
                const auto index2x = CEDD::index_2d_arrayC(xx, yy, side_len);
                const auto index_x1 = CEDD::index_2d_arrayC(xx + 1, yy, side_len);
                const auto index_y1 = CEDD::index_2d_arrayC(xx, yy + 1, side_len);
                const auto index_yx1 = CEDD::index_2d_arrayC(xx + 1, yy + 1, side_len);

                resultMatrix[index] = (inputMatrix[index2x] + inputMatrix[index_x1] +
                                       inputMatrix[index_y1] + inputMatrix[index_yx1]) /
                                      4;

                double vertDiff = (-inputMatrix[index2x] - inputMatrix[index_x1] +
                                   inputMatrix[index_y1] + inputMatrix[index_yx1]);

                double horzDiff = (inputMatrix[index2x] - inputMatrix[index_x1] +
                                   inputMatrix[index_y1] - inputMatrix[index_yx1]);

                double diagDiff = (-inputMatrix[index2x] + inputMatrix[index_x1] +
                                   inputMatrix[index_y1] - inputMatrix[index_yx1]);

                resultMatrix[CEDD::index_2d_arrayC(x + xOffset, y, side_len)] =
                    static_cast<int8_t>(multiplier + std::abs(vertDiff));

                resultMatrix[CEDD::index_2d_arrayC(x, y + yOffset, side_len)] =
                    static_cast<int8_t>(multiplier + std::abs(horzDiff));

                resultMatrix[CEDD::index_2d_arrayC(
                    x + xOffset, y + yOffset, side_len)] =
                    static_cast<int8_t>(multiplier + std::abs(diagDiff));
            }
            else
            {
                if ((x >= side_len / level) || (y >= side_len / level))
                {
                    const auto index = CEDD::index_2d_arrayC(x, y, side_len);
                    resultMatrix[index] = inputMatrix[index];
                }
            }
        }
    }

    double Temp1 = 0;
    double Temp2 = 0;
    double Temp3 = 0;

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            Temp1 +=
                0.25 *
                std::pow(resultMatrix[CEDD::index_2d_arrayC(2 + i, j, side_len)], 2);
            Temp2 +=
                0.25 *
                std::pow(resultMatrix[CEDD::index_2d_arrayC(i, 2 + j, side_len)], 2);
            Temp3 +=
                0.25 *
                std::pow(resultMatrix[CEDD::index_2d_arrayC(2 + i, 2 + j, side_len)],
                         2);
        }
    }
    TempMatrix.F1 = std::sqrt(Temp1);
    TempMatrix.F2 = std::sqrt(Temp2);
    TempMatrix.F3 = std::sqrt(Temp3);
    TempMatrix.Entropy = 0;

    return TempMatrix;
}

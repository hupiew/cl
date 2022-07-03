// jcd.cpp
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

#include <jcd.h>

#include <limits>

#include <QImage>

#include <cedd.h>
#include <fcth.h>

void JCD::joinHistograms(const CEDD& cedd, const FCTH& fcth)
{
    const auto fcth_data = fcth.get_data();
    const auto cedd_data = cedd.get_data();

    for (int i = 0; i < 24; i++)
    {
        data[0 + i] = fcth_data[0 + i] + fcth_data[96 + i];
        data[24 + i] = fcth_data[24 + i] + fcth_data[120 + i];
        data[72 + i] = fcth_data[48 + i] + fcth_data[144 + i];
        data[120 + i] = fcth_data[72 + i] + fcth_data[168 + i];
    }

    for (int i = 0; i < 24; i++)
    {
        data[i] = (data[i] + cedd_data[i]) / 2;
        data[24 + i] = (data[24 + i] + cedd_data[48 + i]) / 2;
        data[48 + i] = cedd_data[96 + i];
        data[72 + i] = (data[72 + i] + cedd_data[72 + i]) / 2;
        data[96 + i] = cedd_data[120 + i];
        data[144 + i] = cedd_data[24 + i];
    }
}

JCD::JCD(const CEDD& cedd, const FCTH& fcth) { joinHistograms(cedd, fcth); }

JCD::JCD() {}

void JCD::extract(const QImage& image)
{
    CEDD cedd{};
    FCTH fcth{};
    cedd.extract(image);
    fcth.extract(image);
    joinHistograms(cedd, fcth);
}

std::vector<int8_t> JCD::get_descriptor() const
{
    // Negative number that counts the amount of sequential (not total) zeros.
    int8_t zero_count = 0;

    auto result = std::vector<int8_t>();
    for (const auto& item : data)
    {
        if (item > 0)
        {
            if (zero_count)
            {
                result.push_back(zero_count);
                zero_count = 0;
            }
            result.push_back(static_cast<int8_t>(2 * item));
        }
        else if (zero_count == std::numeric_limits<int8_t>::min())
        {
            // prevent underflow (undefined behavior)
            result.push_back(zero_count);
            zero_count = -1;
        }
        else
        {
            zero_count--;
        }
    }
    if (zero_count)
    {
        result.push_back(zero_count);
    }
    return result;
}

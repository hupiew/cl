// fcthquant.h
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



class FcthQuant
{
    static constexpr int quant_len = 8 * 8; // 8 items * 8 tables
    static constexpr double QuantTable[quant_len] = {
        130.0887781556944,
        9317.31301788632,
        22434.355689233365,
        43120.548602722061,
        83168.640165905046,
        101430.52589975641,
        174840.65838706805,
        224480.41479670047,
        //};
        // static constexpr double QuantTable2[quant_len] = {
        130.0887781556944,
        9317.31301788632,
        22434.355689233365,
        43120.548602722061,
        83168.640165905046,
        151430.52589975641,
        174840.65838706805,
        224480.41479670047,
        //};
        // static constexpr double QuantTable3[quant_len] = {
        239.769468748322,
        17321.704312335689,
        39113.643180734696,
        69333.512093874378,
        79122.46400035513,
        90980.3325940354,
        161795.93301552488,
        184729.98648386425,
        //};
        // static constexpr double QuantTable4[quant_len] = {
        239.769468748322,
        17321.704312335689,
        39113.643180734696,
        69333.512093874378,
        79122.46400035513,
        90980.3325940354,
        161795.93301552488,
        184729.98648386425,
        //};
        // static constexpr double QuantTable5[quant_len] = {
        239.769468748322,
        17321.704312335689,
        39113.643180734696,
        69333.512093874378,
        79122.46400035513,
        90980.3325940354,
        161795.93301552488,
        184729.98648386425,
        //};
        // static constexpr double QuantTable6[quant_len] = {
        239.769468748322,
        17321.704312335689,
        39113.643180734696,
        69333.512093874378,
        79122.46400035513,
        90980.3325940354,
        161795.93301552488,
        184729.98648386425,
        //};
        // static constexpr double QuantTable7[quant_len] = {
        180.19686541079636,
        23730.024499150866,
        41457.152912541605,
        53918.55437576842,
        69122.46400035513,
        81980.3325940354,
        91795.93301552488,
        124729.98648386425,
        //};
        // static constexpr double QuantTable8[quant_len] = {
        180.19686541079636,
        23730.024499150866,
        41457.152912541605,
        53918.55437576842,
        69122.46400035513,
        81980.3325940354,
        91795.93301552488,
        124729.98648386425,
    };

public:
    static std::array<double, 192> apply(
        const std::array<double, 192>& Local_Edge_Histogram);

    FcthQuant();
};

// extractor.h
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

#pragma once

#include <cstdint>
#include <vector>

#include <util/BytesView.hpp>

class QImage;


class Extractor
{
public:
    Extractor() = default;

    virtual ~Extractor() = default;

    virtual void extract(const QImage& image) = 0;

    static void escaped_push_back(const imsearch::BytesView&, std::vector<int8_t>*);

    virtual std::vector<int8_t> get_descriptor() const = 0;

    virtual bool is_variable() const noexcept { return true; }
};

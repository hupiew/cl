/** extractor.cpp
   CL
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

#include <extractor.h>

#include <cstdint>
#include <limits.h>
#include <vector>

#include <util/BytesView.hpp>

void Extractor::escaped_push_back(const imsearch::BytesView& view,
                                  std::vector<int8_t>* vec)
{
    const int8_t* bytes = reinterpret_cast<const int8_t*>(view.ptr);
    for (int i = 0; i < view.size; ++i)
    {
        if (bytes[i] == std::numeric_limits<int8_t>::min())
            vec->push_back(-127); // if it is -128 we escape the value.
        else if (bytes[i] == -127)
            vec->push_back(-127); // if it is the escape char escape it.

        vec->push_back(bytes[i]);
    }
}

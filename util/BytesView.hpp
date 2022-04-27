// BytesView.hpp
/* ImSearch
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
// https://gitlab.com/Hupie69xd/imsearch/-/blob/b9ccb4b161e65cbbc63e0fc15c568e2284b109cd/util/BytesView.hpp

#pragma once

#include <type_traits>
#include <cstddef>
#include <cstdint>
#include <stdexcept>


namespace imsearch {


struct BytesView
{
    const uint8_t* ptr;
    const size_t size;


    template<typename T>
    explicit BytesView(const T& obj)
      : ptr(reinterpret_cast<const uint8_t*>(obj.data()))
      , size(obj.size() * (sizeof *obj.data())) // Is that mult a good idea?
    {
        if (ptr == nullptr) throw std::runtime_error("ptr can't be null.");
    }

    BytesView() = delete;
    BytesView(const void* p, size_t s)
      : ptr(reinterpret_cast<const uint8_t*>(p))
      , size(s)
    {
        if (ptr == nullptr) throw std::runtime_error("ptr can't be null.");
    }

    const uint8_t* cbegin() { return ptr; }
    const uint8_t* cend() { return ptr + size; }
};

} // namespace imsearch

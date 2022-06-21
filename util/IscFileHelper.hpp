// IscFileHelper.hpp
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
// https://gitlab.com/Hupie69xd/imsearch/-/blob/b9ccb4b161e65cbbc63e0fc15c568e2284b109cd/util/IscFileHelper.cpp

#pragma once

#include <string>
#include <utility>
#include <vector>

#include <util/BytesView.hpp>


namespace imsearch {


struct IscIndexHelper;


class IscFileHelper
{
    std::vector<IscIndexHelper> indecies{};
    std::string name{};
    std::string zip_id{};
    float fps{};

public:
    void add_index(IscIndexHelper input);

    void set_name(const std::string& name);
    void set_zip_id(const std::string& zip_id);
    void set_fps(float fps);

    std::vector<uint8_t> to_file();
};



struct IscIndexHelper
{
    int desc_len{ 1 };
    int length{ -1 };
    char type{ '\xff' };
    std::vector<uint8_t> hashdata{};
    std::vector<uint32_t> imageindex{};
    std::vector<uint32_t> timeindex{};
    std::vector<std::pair<uint32_t, std::string>> titles{};


    void add_hashdata(const imsearch::BytesView input);

    void add_imageindex(const uint32_t* input, size_t length);
    void add_imageindex(const std::vector<uint32_t>& image)
    {
        return add_imageindex(&image[0], image.size());
    }
    void add_timeindex(const uint32_t* input, size_t length);
    void add_timeindex(const std::vector<uint32_t>& times)
    {
        return add_timeindex(&times[0], times.size());
    }

    void add_title(const std::string& title, int offset);

    void set_descriptor_length(int size);
    void set_length(int length);
    void set_type(unsigned char type);
};

} // namespace imsearch

// IscFileHelper.cpp
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

#include <util/IscFileHelper.hpp>

#include <algorithm>
#include <iterator>

#include <IscFile_generated.h>

#include <flatbuffers/flatbuffers.h>


namespace imsearch {


void IscIndexHelper::add_hashdata(const imsearch::BytesView input)
{
    std::copy_n(input.ptr, input.size, std::back_inserter(hashdata));
}

void IscIndexHelper::add_imageindex(const uint32_t* input, size_t length)
{
    std::copy_n(input, length, std::back_inserter(imageindex));
}

void IscIndexHelper::add_timeindex(const uint32_t* input, size_t length)
{
    std::copy_n(input, length, std::back_inserter(timeindex));
}

void IscIndexHelper::add_title(const std::string& title, int offset)
{
    titles.emplace_back(offset, title);
}

void IscIndexHelper::set_descriptor_length(int size)
{
    desc_len = size; }

void IscIndexHelper::set_length(int a_length) { length = a_length; }

void IscIndexHelper::set_type(unsigned char type)
{
    this->type = type;
}

void IscFileHelper::add_index(IscIndexHelper input)
{
    indecies.push_back(input);
}

void IscFileHelper::set_name(const std::string& name)
{
    this->name = name;
}

void IscFileHelper::set_zip_id(const std::string& zip_id)
{
    this->zip_id = zip_id;
}

void IscFileHelper::set_fps(float fps)
{
    this->fps = fps;
}

std::vector<uint8_t> IscFileHelper::to_file()
{
    flatbuffers::FlatBufferBuilder builder{};

    std::vector<flatbuffers::Offset<IscIndex>> index_offsets{};
    for (const auto& item : indecies)
    {
        std::vector<flatbuffers::Offset<IscTitle>> title_offsets{};
        for (const auto& title : item.titles)
        {
            auto offset = CreateIscTitleDirect(builder, title.first, title.second.c_str());
            title_offsets.push_back(offset);
        }
        const size_t count = (item.length == -1) ?
            item.hashdata.size() / item.desc_len :
            item.length;
        const auto index = CreateIscIndexDirect(builder,
            //0, // Separator
            count, // amount of hashes / descriptors
            item.type, // type number
            //nullptr, // uint64 hashes vector (depracated)
            &item.timeindex,
            &item.imageindex,
            &title_offsets,
            &item.hashdata);

        index_offsets.push_back(index);
    }

    const auto cf_indecies = builder.CreateVector(index_offsets);

    const auto name = builder.CreateString(this->name);
    const auto zip_id = builder.CreateString(this->zip_id);

    const auto finished_cfile = CreateIscFile(builder, name, zip_id, this->fps, cf_indecies);
    builder.Finish(finished_cfile, "ISCF");

    // write and compress data
    uint8_t* buf = builder.GetBufferPointer();
    const int size = builder.GetSize();

    return std::vector<uint8_t>{ buf, buf + size };
}

} // namespace imsearch

// phash.h
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

#include <extractor.h>


class PHash : public Extractor
{
public:
    PHash();
    // Length of a side of our square image
    static constexpr int SIDE = 32;

    virtual void extract(const QImage& image) override;

    virtual std::vector<int8_t> get_descriptor() const override;

    virtual bool is_variable() const noexcept override { return false; }

private:
    std::vector<float> dct_cv(const QImage& image);
    void dct_step(std::vector<float>* input, int start, int step);
    void dct_scale(std::vector<float>* input);

    uint64_t hash;
};

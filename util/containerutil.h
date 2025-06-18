#pragma once


struct ContainerUtil
{
    static size_t index_2d(size_t column, size_t row, size_t width)
    {
        // Row-major order
        return row * width + column;
    }
};

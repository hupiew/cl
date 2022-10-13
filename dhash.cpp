/** dhash.cpp
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

#include <dhash.h>

#include <QImage>

DHash::DHash()
{
}

void DHash::extract(const QImage& image)
{
    // We do a faster rougher transform first because it's saves a lot of time.
    // For the same reason as PHash does it, in this case I didn't measure though.
    auto interm = image.scaledToHeight(180, Qt::FastTransformation);

    // dHash is very sensitive to small changes, and because scaling isn't very smooth
    // at factors below 0.5 we scale the image down in multiple steps.
    // See also:
    // https://bugreports.qt.io/browse/QTBUG-30682?focusedCommentId=200210#comment-200210
    interm = interm.scaledToHeight(90, Qt::SmoothTransformation);
    interm = interm.scaledToHeight(45, Qt::SmoothTransformation);
    if (26 < interm.width())
    {
        interm = interm.scaledToHeight(22, Qt::SmoothTransformation);
        interm = interm.scaledToHeight(16, Qt::SmoothTransformation);
    }
    const auto resized =
        interm.scaled(9, 8, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    uint64_t hash{ 0 };
    for (int i = 0; i < resized.height(); ++i)
    {
        const QRgb* line = reinterpret_cast<const QRgb*>(resized.scanLine(i));
        double last{ 0 };
        for (int j = 0; j < resized.width(); ++j)
        {
            const QRgb& col = line[j];

            const auto red = qRed(col);
            const auto green = qGreen(col);
            const auto blue = qBlue(col);
            const auto gray = 0.114 * blue + 0.587 * green + 0.299 * red;

            if (j != 0)
            {
                hash = hash << 1;
                hash += (last < gray);
            }
            last = gray;
        }
    }

    this->hash = hash;
}

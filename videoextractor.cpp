// videoextractor.cpp
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

#include <videoextractor.h>

#include <iostream>
#include <stdexcept>
#include <utility>

#include <QVideoFrame>

#include <util/IscFileHelper.hpp>

VideoExtractor::VideoExtractor(QObject* parent)
  : QObject(parent)
  , extract(nullptr)
  , frame_count(0)
{}

imsearch::IscIndexHelper VideoExtractor::get_data(bool set_title) const
{
    auto index = imsearch::IscIndexHelper();
    index.add_hashdata(imsearch::BytesView(data));
    const uint32_t image_index[] = { static_cast<uint32_t>(frame_count), 0 };
    index.add_imageindex(&image_index[0], 2);
    index.add_timeindex(time_data);
    index.set_length(frame_count);
    if (set_title) index.add_title(filename, frame_count);

    return index;
}

std::unique_ptr<Extractor> VideoExtractor::get_extractor()
{
    std::unique_ptr<Extractor> out(nullptr);
    std::swap(out, extract);
    return out;
}


void VideoExtractor::load_video(const QUrl& path)
{
    if (!extract)
    {
        std::cout << "No hasher availible" << '\n';
        return;
    }
    player.setSource(path);
    player.setVideoSink(&sink);
    filename = path.fileName().toStdString();

    QObject::connect(
        &sink, &QVideoSink::videoFrameChanged, this, &VideoExtractor::receiveFrame);

    QObject::connect(&player,
                     &QMediaPlayer::mediaStatusChanged,
                     this,
                     &VideoExtractor::statusChanged);
}


void VideoExtractor::set_extractor(std::unique_ptr<Extractor> extractor)
{
    extract = std::move(extractor);
}


void VideoExtractor::receiveFrame(const QVideoFrame& frame)
{
    const QImage image = frame.toImage();
    extract->extract(image);
    const auto desc = extract->get_descriptor();
    if (desc != last_desc)
    {
        if (extract->is_variable() && 0 < frame_count) data.emplace_back(-128);
        data.insert(data.end(), desc.cbegin(), desc.cend());

        for (const auto& i : desc)
        {
            if (i == -128)
                throw std::runtime_error("To support variable length descriptors more "
                                         "work is needed, complain to Hupie about it.");
        }
        frame_count++;
        while (time_data.size() < player.position() / 1000) // Milliseconds to seconds
            time_data.emplace_back(frame_count);
    }
    last_desc = std::move(desc);
}

void VideoExtractor::statusChanged(QMediaPlayer::MediaStatus status)
{
    qDebug() << "Status changed " << status;
    if (status == QMediaPlayer::LoadedMedia)
    {
        player.play();

        player.setPlaybackRate(2.0);
    }
    else if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::InvalidMedia)
    {
        qDebug() << "Processed " << frame_count << " frames";
        emit processingDone();
    }
}

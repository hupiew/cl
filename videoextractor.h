/** videoextractor.h
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

#pragma once

#include <cstdint>
#include <memory>

#include <QMediaPlayer>
#include <QObject>
#include <QUrl>
#include <QVideoFrame>
#include <QVideoSink>

#include <extractor.h>

namespace imsearch {
struct IscIndexHelper;
}


class VideoExtractor : public QObject
{
    Q_OBJECT;

public:
    explicit VideoExtractor(QObject* parent = nullptr);

    imsearch::IscIndexHelper get_data(bool set_title, uint8_t type) const;

    std::unique_ptr<Extractor> get_extractor();

    void load_video(const QUrl& path);

    void set_extractor(std::unique_ptr<Extractor> extractor);

public slots:
    void receiveFrame(const QVideoFrame& frame);

    void statusChanged(QMediaPlayer::MediaStatus status);

    void durationChanged(qint64 duration);

signals:
    void processingDone();

private:
    QMediaPlayer* player;
    QVideoSink* sink;
    std::unique_ptr<Extractor> extract;
    qint64 duration;
    int frame_count;

    std::vector<int8_t> data;
    std::vector<int8_t> last_desc;
    std::vector<uint32_t> time_data;
    std::string filename;
};

// videoextractorffmpeg.hpp
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

#include <memory>
#include <string>

#include <QImage>
#include <QUrl>
extern "C"
{
#include <libavutil/avutil.h>
#include <libavutil/pixfmt.h>

    struct AVCodecContext;
    struct AVFormatContext;
    struct AVFrame;
    struct AVPacket;
    struct AVStream;
    struct SwsContext;
}

#include <extractor.h>

namespace imsearch {
struct IscIndexHelper;
}


class VideoExtractorFFmpeg
{
    std::vector<int8_t> data;
    std::vector<int8_t> last_desc;
    std::vector<uint32_t> time_data;
    std::unique_ptr<Extractor> extract;
    std::string filename;
    int64_t duration;
    int frame_count;
    int width;
    int height;


    // FFmpeg data
    AVFormatContext* fmt_ctx = nullptr;
    AVCodecContext* video_dec_ctx = nullptr;
    AVPixelFormat pix_fmt;
    AVFrame* frame;
    AVFrame* dest;
    AVPacket* pkt;
    SwsContext* sws_context = nullptr;
    int video_stream_idx = -1;

    void throw_error(const char* what = "Error opening video.");

public:
    VideoExtractorFFmpeg();
    ~VideoExtractorFFmpeg();

    imsearch::IscIndexHelper get_data(bool set_title, uint8_t type) const;

    std::unique_ptr<Extractor> get_extractor();

    void load_video(const QUrl& path);

    void set_extractor(std::unique_ptr<Extractor> extractor);

private:
    void main_loop();

    int decode_packet(AVFrame* frame, AVCodecContext* dec, const AVPacket* pkt);

    void extract_image(const AVFrame* frame);

    static int open_codec_context(int* stream_idx,
                                  AVCodecContext** dec_ctx,
                                  AVFormatContext* fmt_ctx,
                                  enum AVMediaType type);

    QImage to_qimage(const AVFrame& frame);
};

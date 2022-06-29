// videoextractorffmpeg.cpp
// This file is addapted from FFmpeg/doc/examples/demuxing_decoding.c
// with the changes I made licened under GPLv3 or later.
/*
 * Copyright (c) 2012 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
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

#include <videoextractorffmpeg.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

#include <util/IscFileHelper.hpp>



void VideoExtractorFFmpeg::throw_error(const char* what)
{
    throw std::runtime_error(what);
}

VideoExtractorFFmpeg::VideoExtractorFFmpeg()
  : extract(nullptr)
  , duration(-1)
  , frame_count(0)
  , width(0)
  , height(0)
  , frame(av_frame_alloc())
  , dest(av_frame_alloc())
  , sw_frame(nullptr)
  , pkt(av_packet_alloc())
  , using_hw(false)
{
    if (!frame) throw_error();
    if (!dest) throw_error();
    if (!pkt) throw_error();
}

VideoExtractorFFmpeg::~VideoExtractorFFmpeg()
{
    if (fmt_ctx) avformat_close_input(&fmt_ctx);
    if (video_dec_ctx) avcodec_free_context(&video_dec_ctx);
    if (frame) av_frame_free(&frame);
    if (dest) av_frame_free(&dest);
    if (sw_frame) av_frame_free(&sw_frame);
    if (pkt) av_packet_free(&pkt);
    if (sws_context) sws_freeContext(sws_context);
    if (hw_frame_buff) av_buffer_unref(&hw_frame_buff);
}

imsearch::IscIndexHelper VideoExtractorFFmpeg::get_data(bool set_title,
                                                        uint8_t type) const
{
    auto index = imsearch::IscIndexHelper();
    index.add_hashdata(imsearch::BytesView(data));
    const uint32_t image_index[] = { static_cast<uint32_t>(frame_count), 0 };
    index.add_imageindex(&image_index[0], 2);
    index.add_timeindex(time_data);
    index.set_length(frame_count);
    index.set_type(type);
    if (set_title) index.add_title(filename, frame_count);

    return index;
}

std::unique_ptr<Extractor> VideoExtractorFFmpeg::get_extractor()
{
    return std::move(extract);
}

void VideoExtractorFFmpeg::load_video(const QUrl& path)
{
    if (!filename.empty()) throw_error("This class has no support for reuse.");

    filename = path.toLocalFile().toStdString();

    if (avformat_open_input(&fmt_ctx, filename.c_str(), nullptr, nullptr) != 0)
        throw_error();
    if (avformat_find_stream_info(fmt_ctx, nullptr) != 0) throw_error();

    main_loop();
}

void VideoExtractorFFmpeg::set_extractor(std::unique_ptr<Extractor> extractor)
{
    extract = std::move(extractor);
}

void VideoExtractorFFmpeg::main_loop()
{
    int ret = 0;

    if (open_codec_context(&video_stream_idx, &video_dec_ctx, fmt_ctx) != 0)
        throw_error("Could not open codec context.");

    const auto stream = fmt_ctx->streams[video_stream_idx];
    if (stream)
    {
        auto dur = std::max(stream->duration, duration);

        duration = (dur * stream->time_base.num) / stream->time_base.den;
    }

    width = video_dec_ctx->width;
    height = video_dec_ctx->height;

    /* read frames from the file */
    while (av_read_frame(fmt_ctx, pkt) == 0)
    {
        // check if the packet belongs to a stream we are interested in, otherwise
        // skip it
        if (pkt->stream_index == video_stream_idx)
            ret = decode_packet(frame, video_dec_ctx, pkt);

        av_packet_unref(pkt);
        if (ret != 0) break;
    }
    std::cout << "Saving " << frame_count << " hashes     \n"
              << "from roughly " << time_data.size() + 1 << " seconds of video\n";
#if defined(CL_PERFORMANCE_BENCHMARK)
    std::cout << "Decode took " << decode_time << ", desc took " << desc_time
              << ", convert took " << convert_time << '\n';
#endif

    /* flush the decoders */
    decode_packet(frame, video_dec_ctx, nullptr);
}


int VideoExtractorFFmpeg::decode_packet(AVFrame* frame,
                                        AVCodecContext* dec,
                                        const AVPacket* pkt)
{
#if defined(CL_PERFORMANCE_BENCHMARK)
    const auto start_d = std::chrono::high_resolution_clock::now();

#endif
    int ret = avcodec_send_packet(dec, pkt);

#if defined(CL_PERFORMANCE_BENCHMARK)
    const auto end_d = std::chrono::high_resolution_clock::now();
    decode_time +=
        std::chrono::duration_cast<std::chrono::microseconds>(end_d - start_d).count();

#endif

    // get all the available frames from the decoder
    while (!ret)
    {
        ret = avcodec_receive_frame(dec, frame);
        if (ret)
        {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) return 0;

            std::cerr << "Error during decoding\n";
            return ret;
        }

        // skip if it isn't a frame
        if (dec->codec->type != AVMEDIA_TYPE_VIDEO) continue;

        if (using_hw)
        {
            av_hwframe_transfer_data(sw_frame, frame, 0);
            av_frame_copy_props(sw_frame, frame);
            extract_image(sw_frame);
            av_frame_unref(frame);
            av_frame_unref(sw_frame);
        }
        else
        {
            extract_image(frame);
            av_frame_unref(frame);
        }
    }
    return 0;
}

void VideoExtractorFFmpeg::extract_image(const AVFrame* frame)
{
    auto image = to_qimage(*frame);

    const auto stream = fmt_ctx->streams[video_stream_idx];

    const auto seconds = (stream->time_base.num * frame->pts) / stream->time_base.den;
    std::cout << "Processing " << seconds << "/" << duration << "(s)\r" << std::flush;

#if defined(CL_PERFORMANCE_BENCHMARK)
    const auto start_d = std::chrono::high_resolution_clock::now();

#endif
    const QImage scaled = image.scaledToHeight(480);
    if (scaled.isNull()) return;

    extract->extract(scaled);
#if defined(CL_PERFORMANCE_BENCHMARK)
    const auto end_d = std::chrono::high_resolution_clock::now();
    desc_time +=
        std::chrono::duration_cast<std::chrono::microseconds>(end_d - start_d).count();

#endif
    const auto desc = extract->get_descriptor();
    if (desc != last_desc)
    {
        if (extract->is_variable() && 0 < frame_count) data.emplace_back(-128);
        data.insert(data.end(), desc.cbegin(), desc.cend());

        for (const auto& i : desc)
        {
            if (i == -128)
            {
                std::cerr << "To support variable length descriptors more work is "
                             "needed, complain to Hupie about it.";
                throw std::runtime_error("Variable descriptors not implemented.");
            }
        }
        frame_count++;
        while (time_data.size() < seconds) time_data.emplace_back(frame_count);
    }
    last_desc = std::move(desc);
}

int VideoExtractorFFmpeg::hw_decode_init(AVCodecContext* context)
{
    int ret =
        av_hwdevice_ctx_create(&hw_frame_buff, AV_HWDEVICE_TYPE_VDPAU, NULL, NULL, 0);
    int device = AV_HWDEVICE_TYPE_VDPAU;

    if (ret != 0)
    {
        ret = av_hwdevice_ctx_create(
            &hw_frame_buff, AV_HWDEVICE_TYPE_DXVA2, NULL, NULL, 0);
        device = AV_HWDEVICE_TYPE_DXVA2;
    }
    sw_frame = av_frame_alloc();

    if (ret != 0 || !sw_frame)
    {
        std::cerr << "Unable to use hardware decoding.";
        return -1;
    }
    context->hw_device_ctx = av_buffer_ref(hw_frame_buff);

    using_hw = true;
    return device;
}

int VideoExtractorFFmpeg::open_codec_context(int* stream_idx,
                                             AVCodecContext** dec_ctx,
                                             AVFormatContext* fmt_ctx)
{
    constexpr auto type = AVMEDIA_TYPE_VIDEO;

    const AVCodec* dec = nullptr;
    int ret = av_find_best_stream(fmt_ctx, type, -1, -1, &dec, 0);
    if (ret)
    {
        std::cerr << "Could not find " << av_get_media_type_string(type)
                  << "stream in input file\n";
        return ret;
    }

    int stream_index = ret;
    AVStream* st = fmt_ctx->streams[stream_index];

    /* find decoder for the stream */
    // not done for hw
    // dec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!dec)
    {
        std::cerr << "Failed to find codec " << av_get_media_type_string(type) << '\n';
        return AVERROR(EINVAL);
    }

    /* Allocate a codec context for the decoder */
    *dec_ctx = avcodec_alloc_context3(dec);
    if (!*dec_ctx)
    {
        std::cerr << "Failed to allocate the " << av_get_media_type_string(type)
                  << " codec context\n";
        return AVERROR(ENOMEM);
    }

    /* Copy codec parameters from input stream to output codec context */
    if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0)
    {
        std::cerr << "Failed to copy " << av_get_media_type_string(type)
                  << "codec parameters to decoder context\n";
        return ret;
    }

    // ******** Hardware stuff
    ret = hw_decode_init(*dec_ctx);
    if (0 < ret)
    {
        auto device = AVHWDeviceType(ret);
        for (int i = 0;; i++)
        {
            const AVCodecHWConfig* config = avcodec_get_hw_config(dec, i);
            if (!config)
            {
                (*dec_ctx)->hw_device_ctx = nullptr;
                break;
            }
            if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
                config->device_type == device)
            {
                // hw_pixel_format = config->pix_fmt;
                ret = 0;
                break;
            }
        }
    }
    if (ret)
        std::cerr << "Could not use hardware decode, using software decode...\n";


    /* Init the decoders */
    if ((ret = avcodec_open2(*dec_ctx, dec, NULL)) < 0)
    {
        std::cerr << "Failed to open codec " << av_get_media_type_string(type) << '\n';
        return ret;
    }
    *stream_idx = stream_index;

    return 0;
}

// This fucntion does not work on big-endian computers
QImage VideoExtractorFFmpeg::to_qimage(const AVFrame& frame)
{
#if defined(CL_PERFORMANCE_BENCHMARK)
    const auto start_d = std::chrono::high_resolution_clock::now();

#endif

    // setup
    if (sws_context == nullptr)
    {
        sws_context = sws_getContext(width,
                                     height,
                                     AVPixelFormat(frame.format),
                                     width,
                                     height,
                                     AVPixelFormat(AV_PIX_FMT_RGB32),
                                     SWS_FAST_BILINEAR,
                                     nullptr,
                                     nullptr,
                                     nullptr);
        if (!sws_context)
            throw_error("Could not alloc SwsContext(VideoRead::to_qimage).");
    }

    if (dest->width == 0)
    {
        dest->format = AVPixelFormat(AV_PIX_FMT_RGB32);
        dest->width = width;
        dest->height = height;
        if (av_frame_get_buffer(dest, 0))
            throw_error("Could not alloc frame buffer(VideoRead::to_qimage).");
    }

    if (frame.format != AV_PIX_FMT_RGB32)
    {
        auto slice = sws_scale(sws_context,
                               frame.data,
                               frame.linesize,
                               0,
                               height,
                               dest->data,
                               dest->linesize);

        if (slice != height) throw_error("Output height does not match input height");
        // qDebug() << dest->data[0] << width << height << dest->linesize[0] << slice;
    }
#if defined(CL_PERFORMANCE_BENCHMARK)
    const auto end_d = std::chrono::high_resolution_clock::now();
    convert_time +=
        std::chrono::duration_cast<std::chrono::microseconds>(end_d - start_d).count();

#endif
    //  QImage does NOT make a copy of dest->data
    return { dest->data[0], width, height, dest->linesize[0], QImage::Format_RGB32 };
}

//
//  sceneVideo.cpp
//  wallpaper64mt
//
//  Created by apple on 2025-09-24.
//  Copyright © 2025 Apple. All rights reserved.
//

#include "scene.hpp"

void VideoRender::init(const std::string& videoPath) {
    if (!std::filesystem::exists(videoPath))
        return;

    avformat_open_input(&format_ctx, videoPath.data(), nullptr, nullptr);
    avformat_find_stream_info(format_ctx, nullptr);

    for (unsigned int i = 0; i < format_ctx->nb_streams; ++i) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    AVCodecParameters* codecpar = format_ctx->streams[video_stream_idx]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);

    avcodec_open2(codec_ctx, codec, nullptr);

    material.width = codec_ctx->width;
    material.height = codec_ctx->height;
    
    material.texture = new MTLTexture();
    material.texture->create(material.width, material.height, MTL::PixelFormatBGRA8Unorm, MTL::TextureType2D, MTL::StorageModeManaged, MTL::TextureUsageShaderRead);
    
    frame2 = av_frame_alloc();
    av_image_alloc(frame2->data, frame2->linesize, material.width, material.height, AV_PIX_FMT_RGB32, 32);
                
    frame = av_frame_alloc();

    ot = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    codec_ctx->time_base = av_make_q(1, 60000);
                        
}

void VideoRender::update() {
    AVPacket packet;
     // Read frames from the video stream
    if (av_read_frame(format_ctx, &packet) >= 0) {

        if (packet.stream_index == video_stream_idx) {
            int ret = avcodec_send_packet(codec_ctx, &packet);
            
            while (ret >= 0) {
                ret = avcodec_receive_frame(codec_ctx, frame);

                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    break;
                }

                if (!converter_ctx) {
                    converter_ctx = sws_getContext(frame->width, frame->height, codec_ctx->pix_fmt,
                                                   frame->width, frame->height, AV_PIX_FMT_RGB32, SWS_BILINEAR,
                                                                            nullptr, nullptr, nullptr);
                }
                
                sws_scale(converter_ctx, frame->data, frame->linesize, 0, codec_ctx->height, frame2->data, frame2->linesize);
                material.texture->upload(4, frame2->data[0]);
            }
        }

        av_packet_unref(&packet);

    }
    else {
        avcodec_flush_buffers(codec_ctx);
        av_seek_frame(format_ctx, video_stream_idx, 0, AVSEEK_FLAG_BACKWARD);
    }

}

void VideoRender::destroy() {
    av_freep(frame2->data);
    av_frame_free(&frame);
    av_frame_free(&frame2);


    sws_freeContext(converter_ctx);
    converter_ctx = nullptr;
    
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);

}

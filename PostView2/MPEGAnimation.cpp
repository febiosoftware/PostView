#include "stdafx.h"
#include "MPEGAnimation.h"

#ifdef FFMPEG
CMPEGAnimation::CMPEGAnimation()
{
    file = NULL;
    av_codec_context = NULL;
    av_output_format = NULL;
    rgb_frame = NULL;
    yuv_frame = NULL;
    av_format_context = NULL;
}

int CMPEGAnimation::Create(const char *szfile, int cx, int cy, float fps)
{
	m_nframe = 0;
    
    avcodec_register_all();
    
    // find mpeg1 video encoder
    av_codec = avcodec_find_encoder(AV_CODEC_ID_MPEG1VIDEO);
    
    if (!av_codec)
    {
        return false;
    }
    
    // alloc an AVCodecContext
    av_codec_context = avcodec_alloc_context3(av_codec);
    
    if (!av_codec_context)
    {
        return false;
    }
    
    av_codec_context->bit_rate = 400000;
    
    // resolution must be a multiple of 2
    av_codec_context->width = cx;
    av_codec_context->height = cy;
    
    // frames per second
    av_codec_context->time_base = av_make_q(1,25);
    
    // emit one intra frame every ten frames
    av_codec_context->gop_size = 10;
    av_codec_context->max_b_frames = 1;
    av_codec_context->pix_fmt = AV_PIX_FMT_YUV420P;
    
    // open the codec
    if (avcodec_open2(av_codec_context, av_codec, NULL))
    {
        return false;
    }
    
    file = fopen(szfile, "wb");
    
    if (!file)
    {
        return false;
    }
    
    return true;
}

int CMPEGAnimation::Write(CRGBImage &im)
{
    // cannot convert rgb24 to yuv420
    if (!Rgb24ToYuv420p(im))
    {
        return false;
    }
    
    int ret;
    int got_packet = 0;
	av_init_packet(&av_packet);
    av_packet.data = NULL;
    av_packet.size = 0;
        
    fflush(stdout);
        
    yuv_frame->pts = m_nframe++;
        
    // encode the pix
    ret = avcodec_encode_video2(av_codec_context, &av_packet, yuv_frame, &got_packet);
        
    if (ret < 0)
    {
        return false;
    }
        
    if (got_packet)
    {
        fwrite(av_packet.data, 1, av_packet.size, file);
        av_free_packet(&av_packet);
    }
    
    return true;
}

bool CMPEGAnimation::Rgb24ToYuv420p(CRGBImage &im)
{
	im.FlipY();

    yuv_frame = av_frame_alloc();
    
    int yuv_frame_bytes = avpicture_get_size(PIX_FMT_YUV420P, av_codec_context->width, av_codec_context->height);
    
    buffer = (uint8_t *) av_malloc(yuv_frame_bytes * sizeof(uint8_t));
    
    avpicture_fill((AVPicture *)yuv_frame, buffer, PIX_FMT_YUV420P, av_codec_context->width, av_codec_context->height);
    
    struct SwsContext *converted_format = NULL;
    
    converted_format = sws_getCachedContext(converted_format, av_codec_context->width, av_codec_context->height, AV_PIX_FMT_RGB24, av_codec_context->width, av_codec_context->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    
    if (!converted_format)
    {
        return false;
    }
    
    int linesize[] = {3 * im.Width(), 0, 0, 0, 0, 0, 0, 0};
	const uint8_t* p = im.GetBytes();
    
    sws_scale(converted_format, &p, linesize, 0, av_codec_context->height, yuv_frame->data, yuv_frame->linesize);
    
    //sws_scale(converted_format, (const uint8_t* const*)rgb_frame->data, rgb_frame->linesize, 0, av_codec_context->height, yuv_frame->data, yuv_frame->linesize);
    
    return true;
}

void CMPEGAnimation::Close()
{
    // get the delayed frames
	int got_packet;
	do
    {
        fflush(stdout);
        
        int ret = avcodec_encode_video2(av_codec_context, &av_packet, NULL, &got_packet);
        
        if (ret < 0)
        {
            break;
        }
        
        if (got_packet)
        {
            fwrite(av_packet.data, 1, av_packet.size, file);
            av_free_packet(&av_packet);
        }
    }
    while (got_packet);

    // add sequence end code to have a real video file
    uint8_t endcode[] = {0, 0, 1, 0xb7};
    fwrite(endcode, 1, sizeof(endcode), file);

    // deallocating AVCodecContext
    avcodec_close(av_codec_context);
    av_free(av_codec_context);
    if (file) fclose(file);
    avcodec_free_frame(&yuv_frame);
    avcodec_free_frame(&rgb_frame);
    
    file = NULL;
    av_codec_context = NULL;
    av_output_format = NULL;
    rgb_frame = NULL;
    yuv_frame = NULL;
    av_format_context = NULL;
}
#endif

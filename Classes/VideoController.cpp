#include "VideoController.h"
#include "string.h"
/*
video = VideoController::create();
addChild(video);
video->startWork();
video->stopWork();
*/


VideoController *VideoController::create()
{
    CCLog("create VideoController");
    VideoController *pRet = new VideoController();
    pRet->init();
    pRet->autorelease();
    return pRet;
}

//时间2s钟
//每秒10帧
bool VideoController::init()
{
    CCLog("init VideoController");
    MaxRecordTime = 1000;
    frameRate = 1./25;  //0.1s 1帧
    startYet = false;
    testTime = 10;

    scheduleUpdate();
    
    int fmt1 = avcodec_pix_fmt_to_codec_tag(PIX_FMT_RGB32_1);	
    //int fmt2 = avcodec_pix_fmt_to_codec_tag(CODEC_ID_MPEG2VIDEO);	
    //int fmt3 = avcodec_pix_fmt_to_codec_tag(CODEC_ID_MPEG4);	
    CCLog("pixel fmt %d", fmt1);
    return true;
}

void VideoController::startWork(int winW, int winH, int w, int h, char *fileName, float fr)
{
    winWidth = winW;
    winHeight = winH;

    width = w;
    height = h;
    tempCache = malloc(sizeof(int)*w);
    startYet = true;
    frameRate = 1.0/25; //每s 30 帧

    passTime = frameRate;
    totalTime = 0;
    frameCount = 0;
    av_register_all();
    

    pixelBuffer = (uint8_t *)malloc(sizeof(int)*winWidth*winHeight);
    if(!pixelBuffer) {
        printf("could not pixelBuffer\n");
        exit(1);
    }

    fmt = av_guess_format(NULL, fileName, NULL);
    if(!fmt) {
        printf("guess fmt error %s\n", fileName);
        exit(1);
    }

    printf("alloc context \n");
    oc = avformat_alloc_context();
    oc->oformat = fmt;
    snprintf(oc->filename, sizeof(oc->filename), "%s", fileName);
    printf("add_video_stream \n");
    video_st = add_video_stream(oc, fmt->video_codec, width, height);
    if(av_set_parameters(oc, NULL) < 0) {
        printf("Invalid output format parameters\n");
    }
    av_dump_format(oc, 0, fileName, 1);
    printf("open_video \n");

    open_video(oc, video_st, &outbuf, &outbuf_size, (AVPicture **)&picture);
    //需要输出文件
    if(!(fmt->flags & AVFMT_NOFILE)) {
        if(avio_open(&oc->pb, fileName, AVIO_FLAG_WRITE) < 0) {
            printf("could not open %s \n" , fileName);
        } 
    }

    av_write_header(oc);
}
void VideoController::stopWork()
{
    av_write_trailer(oc);


    avcodec_close(video_st->codec);
    av_free(picture->data[0]);
    av_free(picture);
    av_free(outbuf);

    unsigned int i;
    for(i = 0; i < oc->nb_streams; i++) {
        av_freep(&oc->streams[i]->codec);
        av_freep(&oc->streams[i]);
    }
    if(!(fmt->flags & AVFMT_NOFILE))
        avio_close(oc->pb);
    av_free(oc);


    free(pixelBuffer);
    free(tempCache);

    outbuf = NULL;
    picture = NULL;
    startYet = false;

}
void VideoController::compressCurrentFrame()
{
    AVCodecContext *c = video_st->codec;

    CCLog("compressCurrentFrame", c->width, c->height);
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);    
    

    SwsContext *sctx = sws_getCachedContext (NULL, width, height, PIX_FMT_RGBA, width, height, PIX_FMT_YUV420P, SWS_FAST_BILINEAR, NULL, NULL, NULL);

    //使用不同的算法进行压缩
    int inLineSize[] = {width*4, 0, 0, 0};
    uint8_t *srcSlice[] = {pixelBuffer, NULL, NULL};
    
    //从底向上处理
    // 修正数据方向 行交换方法
    int i, j;
    for(i = 0, j = height-1; i < j; i++, j--) {
        memcpy(tempCache, &pixelBuffer[(i*width)*4], 4*width);
        memcpy(&pixelBuffer[(i*width)*4], &pixelBuffer[(j*width)*4], 4*width);
        memcpy(&pixelBuffer[(j*width)*4], tempCache, 4*width);
    }

    int retCode = sws_scale (sctx, srcSlice, inLineSize, 0, height, picture->data, picture->linesize);
    printf("sws_scale %d\n", retCode);
    sws_freeContext(sctx);

    video_pts = (double)video_st->pts.val*video_st->time_base.num/video_st->time_base.den;


    int ret;
    int out_size = avcodec_encode_video(c, outbuf, outbuf_size, picture);
    //zero size image was buffered
    if(out_size > 0) {
        AVPacket pkt;
        av_init_packet(&pkt);
        //编码器 需要PTS 需要在 codec 的timebase 和 stream的 timebase之间转化
        if(c->coded_frame->pts != AV_NOPTS_VALUE) {
            pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base, video_st->time_base);
        }
        //当前是关键帧
        if(c->coded_frame->key_frame) {
            pkt.flags |= AV_PKT_FLAG_KEY;
        }
        pkt.stream_index = video_st->index;
        pkt.data = outbuf;
        pkt.size = out_size;
        
        //将包数据写入 输出文件
        ret = av_interleaved_write_frame(oc, &pkt);
    } else {
        ret = 0;
    }

    //fwrite(outbuf, 1, out_size, f);
}
/*
pts 帧率的问题
http://stackoverflow.com/questions/6603979/ffmpegavcodec-encode-video-setting-pts-h264
*/
void VideoController::update(float dt)
{
    //CCLog("update %d %f", startYet, dt);
    if(startYet ) {

        if(totalTime < testTime) {//测试时间10s
            totalTime += dt;
            passTime += dt;
            if(passTime >= frameRate) {
                CCLog("update %f %f %f %f", totalTime, MaxRecordTime, passTime, frameRate);
                passTime -=  frameRate;
                compressCurrentFrame();
                frameCount += 1;
            }
        } else {
            stopWork();
        }
    }
}

VideoController::~VideoController()
{

}



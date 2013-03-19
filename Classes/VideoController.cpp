#include "VideoController.h"
#include "string.h"


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
    frameRate = 0.1;  //0.1s 1帧
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
    frameRate = 1.0/30; //每s 30 帧

    passTime = frameRate;
    totalTime = 0;
    frameCount = 0;
    

    pixelBuffer = (uint8_t *)malloc(sizeof(int)*winWidth*winHeight);
    f = fopen(fileName, "wb");
    //c = video_init(width, height, 1/frameRate); //每秒多少帧 mpeg1/2 支持24-30 HZ
     

    fmt = av_guess_format("mp4", NULL, NULL);

    picture = video_initFrame(c, &picture_buf);
    video_initBuf(c, &outbuf_size, &outbuf);


}
void VideoController::stopWork()
{
    free(tempCache);
    outbuf[0] = 0x00;
    outbuf[1] = 0x00;
    outbuf[2] = 0x01;
    outbuf[3] = 0xb7;
    fwrite(outbuf, 1, 4, f);

    video_clean(f, picture_buf, outbuf, c, picture);
    f = NULL;
    picture_buf = NULL;
    outbuf = NULL;
    c = NULL;
    picture = NULL;
    startYet = false;

}
void VideoController::compressCurrentFrame()
{
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
    sws_freeContext(sctx);


    int out_size = avcodec_encode_video(c, outbuf, outbuf_size, picture);
    fwrite(outbuf, 1, out_size, f);
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
                //picture->pts = (1.0/30.0)*90*frameCount;
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
    free(pixelBuffer);
}



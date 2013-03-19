#ifndef __VIDEO_CONTROLLER_H__
#define __VIDEO_CONTROLLER_H__
#include "cocos2d.h"
#include "yuv420.h"
#include "stdlib.h"

using namespace cocos2d;
class VideoController : public CCNode {
public:
    static VideoController *create();
    virtual bool init();
    void startWork(int winW, int winH, int w, int h, char *fn, float fr);
    void stopWork();
    virtual void update(float dt);
    ~VideoController();
    void compressCurrentFrame();
    bool getStart() {
        return startYet;
    }
private:
    float MaxRecordTime; // 最大视频时间
    float frameRate;  //帧率

    
    AVCodecContext *c; //视频编码上下文
    AVFrame *picture; //每一帧
    int outbuf_size;  
    uint8_t *outbuf;  //输出数据缓存
    uint8_t *picture_buf; //每一帧数据YUV 数据
    uint8_t *pixelBuffer; //显卡中的RGBA 数据

    float passTime; //每帧记录的时间
    bool startYet;  //是否开始记录
    FILE *f;        //输出的文件

    float totalTime; //当前总共记录的时间
    //用于像素 缓冲分配
    int winWidth;//屏幕大小
    int winHeight; //屏幕大小
    //用于视频缓冲分配
    int width; //视频的 宽度
    int height; //视频的高度
    void *tempCache;
    int frameCount;

    float testTime;

    AVFormatContext *oc;
    AVOutputFormat *fmt;
    AVStream *video_st;
    double video_pts;
};
#endif

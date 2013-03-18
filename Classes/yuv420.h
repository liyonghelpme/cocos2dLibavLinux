#ifndef __YUV420_H__
#define __YUV420_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "colorspace.h"
#include "libavutil/pixdesc.h"
AVCodecContext *video_init(int width, int height, int frameRate);
AVFrame *video_initFrame(AVCodecContext *c, uint8_t **pb);
void video_initBuf(AVCodecContext *c, int *outbuf_size, uint8_t **outbuf);
void compressPicture(AVCodecContext *c, AVFrame *picture); 
void video_clean(FILE *f, uint8_t *picture_buf, uint8_t *outbuf, AVCodecContext *c, AVFrame *picture);
#ifdef __cplusplus
}
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "yuv420.h"

#ifdef __cplusplus
}
#endif


/*
解码器的上下文
http://stackoverflow.com/questions/3553003/encoding-h-264-with-libavcodec-x264
设置ctx
*/
AVCodecContext *video_init(int width, int height, int frameRate)
{
    av_register_all();
    AVCodec *codec; //编码器
    AVCodecContext *c; //上下文
    
    //压缩视频数据
    codec = avcodec_find_encoder(CODEC_ID_MPEG4);
    int i;
    for(i = 0; codec->pix_fmts[i] != PIX_FMT_NONE; i++)
        printf("pixel format %s\n", av_pix_fmt_descriptors[codec->pix_fmts[i]].name);

    if (!codec) {
        fprintf(stderr, "codec not found\n");
        exit(1);
    }
    //针对该编码器的一个 上下文
    c = avcodec_alloc_context3(codec);

    c->bit_rate = 400000;
    /* resolution must be a multiple of two */
    c->width = width;
    c->height = height;
    /* frames per second 在25的范围内*/
    c->time_base= (AVRational){1, 25};
    c->gop_size = 12; /* emit one intra frame every ten frames */
    //不要双向推测帧
    c->max_b_frames=0;
    c->pix_fmt = PIX_FMT_YUV420P;
    c->profile = FF_PROFILE_MPEG4_SIMPLE;

    //打开编码器的上下文
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "could not open codec\n");
        exit(1);
    }
   
    return c;
}
/*
每一帧的数据
*/
AVFrame *video_initFrame(AVCodecContext *c, uint8_t **pb)
{
    AVFrame *picture;
    picture = avcodec_alloc_frame();
    uint8_t *picture_buf;
    //int size = c->width * c->height;

    int size = avpicture_get_size(c->pix_fmt, c->width, c->height);
    picture_buf = av_malloc(size);
    if(!picture_buf) {
        av_free(picture_buf);
        return NULL;
    }
    avpicture_fill((AVPicture *)picture, picture_buf, c->pix_fmt, c->width, c->height);
    printf("picture_buf %d %d %d %d\n", size, picture->linesize[0], picture->linesize[1],picture->linesize[2] );

    *pb = picture_buf;
    return picture;
}
void video_initBuf(AVCodecContext *c, int *outbuf_size, uint8_t **outbuf)
{
    *outbuf_size = 500000;
    *outbuf = malloc(*outbuf_size);
}

/*
制作一帧数据
*/
void compressPicture(AVCodecContext *c, AVFrame *picture) 
{
    fflush(stdout);
    /* prepare a dummy image */
    /* Y */
    int x, y;
    for(y=0;y<c->height;y++) {
        for(x=0;x<c->width;x++) {
            int r, g, b;
            r = g = b = 0;
            if((x/10) % 3 == 0)
                r = 255;
            if((x/10) % 3 == 1)
                g = 255;
            if((x/10) % 3 == 2)
                b = 255;
            picture->data[0][y*picture->linesize[0]+x] = RGB_TO_Y_CCIR(r, g, b);
        }
    }

    //2 * 2平均值0 1
    //           2 3
    /* Cb and Cr */
    //解析度的一半
    for(y=0;y<c->height/2;y++) {
        for(x=0;x<c->width/2;x++) {
            int r, g, b;
            int col = x*2;
            r = g = b = 0;
            if((col/10) % 3 == 0)
                r = 255;
            if((col/10) % 3 == 1)
                g = 255;
            if((col/10) % 3 == 2)
                b = 255;
            picture->data[1][y * picture->linesize[1] + x] = RGB_TO_U_CCIR(r, g, b, 0);
            picture->data[2][y * picture->linesize[2] + x] = RGB_TO_V_CCIR(r, g, b, 0);
        }
    }
}

//清理分配的数据 init
void video_clean(FILE *f, uint8_t *picture_buf, uint8_t *outbuf, AVCodecContext *c, AVFrame *picture)
{
    fclose(f);
    free(picture_buf);
    free(outbuf);

    avcodec_close(c);
    av_free(c);
    av_free(picture);
}

AVStream *add_video_stream(AVFormatContext *oc, enum CodecID codec_id, int width, int height) 
{
    AVCodecContext *c;
    AVStream *st;
    st = avformat_new_stream(oc, NULL);
    c = st->codec;
    c->codec_id = codec_id;
    c->codec_type = AVMEDIA_TYPE_VIDEO;
    c->bit_rate = 400000;
    c->width = width;
    c->height = height;
    
    c->time_base.den = 25;
    c->time_base.num = 1;
    c->gop_size = 12;
    c->pix_fmt = PIX_FMT_YUV420P;
    c->profile = FF_PROFILE_MPEG4_SIMPLE;
    
    if(oc->oformat->flags & AVFMT_GLOBALHEADER)
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
    return st;
}


static AVFrame *alloc_picture(enum PixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture)
        return NULL;
    //得到picture 大小的函数 格式 宽度 和 高度 YUV420  YUV422 RGB24
    size = avpicture_get_size(pix_fmt, width, height);
    picture_buf = av_malloc(size);
    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }
    //填充 picture frame linesize frame的 data属性
    avpicture_fill((AVPicture *)picture, picture_buf,
                   pix_fmt, width, height);
    return picture;
}

void open_video(AVFormatContext *oc, AVStream *st, uint8_t **outbuf, int *outSize, AVPicture **pic) 
{
    AVCodec *codec;
    AVCodecContext *c;
    uint8_t *video_outbuf;
    int video_outbuf_size;
    AVPicture *picture;

    c = st->codec;
    codec = avcodec_find_encoder(c->codec_id);
    if(!codec) {
        printf("codec not found\n");
        exit(1);
    }
    if(avcodec_open(c, codec) < 0) {
        printf("could not open codec\n");
        exit(1);
    }

    video_outbuf = NULL;
    //输出格式 不是 纯数据 则 需要分配out buffer 来处理
    if(!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
        video_outbuf_size = 200000;
        video_outbuf = av_malloc(video_outbuf_size);
    }
    picture = alloc_picture(c->pix_fmt, c->width, c->height);
    if(!picture) {
        printf("could not allocate picture\n");
        exit(1);
    }
    //不需要一个临时的YUV420P picture
    *outbuf = video_outbuf;
    *outSize = video_outbuf_size;
    *pic = picture;
}


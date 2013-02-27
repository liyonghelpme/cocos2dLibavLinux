#include "Lightning.h"
#include "stdlib.h"
#include "cmath"
#include "kazmath/utility.h"

Line *Line::create(const char *fileName, kmVec3 &a, kmVec3 &b, float thickness, float deg, ccColor3B c, kmVec3 &temp)
{
    Line *line = new Line();
    line->initWithFile(fileName);
    line->setTextureRect(CCRectMake(63, 0, 1, 128));//宽第1的中间像素
    line->a = a;
    line->b = b;

    line->setAnchorPoint(ccp(0, 0.5));
    line->setScaleX(kmVec3Length(&temp)/1.0);
    line->setScaleY(thickness/128);
    line->setPosition(ccp(a.x, a.y));

    line->setRotation(deg);
    line->setColor(c);

    line->autorelease();


    return line;
}

//使用特定的Lightning 图片
//可以使用孩子节点的缓存 而不重新分配CCSprite

Lightning *Lightning::create(const char *fileName, unsigned int capacity, float detail, float thickness, float displace)//根据details 来设定sprite的容量
{
    Lightning *pRet = new Lightning();
    pRet->initWithFile("edge.png", capacity);
    pRet->fileName = "edge.png";
    pRet->lines = CCArray::create();
    pRet->detail = detail;
    pRet->thickness = thickness;
    pRet->displace = displace;
    ccBlendFunc blendFunc = {GL_ONE, GL_ONE};
    pRet->setBlendFunc(blendFunc);
    pRet->alpha = 1.0f;
    pRet->fadeOutRate = 0.03f;


    pRet->autorelease();
    return pRet;
}
Lightning::~Lightning()
{
    lines->release();
}
void Lightning::testLine(float x1, float y1, float x2, float y2)
{
    /*
    kmVec3 a = {x1, y1};
    kmVec3 b = {x2, y2};

    //Line *line = Line::create(this->fileName.c_str(), a, b, thickness); 
    //addChild(line);


    CCSprite *s = CCSprite::create(fileName.c_str());
    s->setAnchorPoint(ccp(0.5, 0.5));
    s->ignoreAnchorPointForPosition(false);
    CCSize size = s->getContentSize();
    printf("%lf, %lf\n", size.width, size.height);
    //s->setPosition(ccp(32, 64));
    //s->setTextureRect(CCRectMake(63, 0, 1, 128));
    //s->setContentSize(CCSizeMake(1, 128));
    //s->setScaleX(20);
    //s->setScaleY(40/128.0);
    addChild(s);

    s = CCSprite::create(fileName.c_str());
    s->setAnchorPoint(ccp(0.5, 0.5));
    s->setPosition(ccp(b.x, b.y));
    //s->setTextureRect(CCRectMake(63, 0, 1, 128));
    //s->setContentSize(CCSizeMake(1, 128));
    s->setScaleX(20);
    //s->setScaleY(40/128.0);
    addChild(s);
    */
}
void Lightning::draw()
{
    if(alpha <= 0)
        return;
    if( m_pobTextureAtlas->getTotalQuads() == 0 )
    {
        return;
    }

    CC_NODE_DRAW_SETUP();

    arrayMakeObjectsPerformSelector(m_pChildren, updateTransform, CCSprite*);

    ccGLBlendFunc( m_blendFunc.src, m_blendFunc.dst );

    m_pobTextureAtlas->drawQuads();
}

void Lightning::midDisplacement(float x1, float y1, float x2, float y2, float dis)
{
    if(dis < detail){
        kmVec3 a, b;
        kmVec3Fill(&a, x1, y1, 0);
        kmVec3Fill(&b, x2, y2, 0);
        
        kmVec3 temp;
        kmVec3Subtract(&temp, &b, &a);
        double deg = -atan2(temp.y, temp.x)*180/kmPI;
        ccColor3B c = {255*.2, 255*.2, 255*.7};


        Line *line = Line::create(this->fileName.c_str(), a, b, thickness, deg, c, temp);
        addChild(line);

        CCSprite *s = CCSprite::create(fileName.c_str());
        s->setAnchorPoint(ccp(1.0, 0.5));
        s->setPosition(ccp(a.x, a.y));
        s->setRotation(deg);
        s->setScale(thickness/128);
        s->setColor(c);
        addChild(s);

        s = CCSprite::create(fileName.c_str());
        s->setAnchorPoint(ccp(0, 0.5));
        s->setPosition(ccp(b.x, b.y));
        s->setRotation(deg);
        s->setFlipX(true);
        s->setScale(thickness/128);
        s->setColor(c);
        addChild(s);

    }else {
        float midX = (x1+x2)/2;
        float midY = (y1+y2)/2;
        midX += (random()*1.0/RAND_MAX-0.5)*dis;
        midY += (random()*1.0/RAND_MAX-0.5)*dis;
        midDisplacement(x1, y1, midX, midY, dis/2);
        midDisplacement(x2, y2, midX, midY, dis/2);
    }
}

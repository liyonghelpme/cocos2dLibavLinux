#include "Background.h"
Background *Background::create()
{
    Background *pRet = new Background();
    pRet->setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));
    pRet->autorelease();
    return pRet;
}
void Background::draw()
{
    CC_NODE_DRAW_SETUP();
    ccDrawColor4F(0.2, 0.2, 0.8, 1);
    for(int i = 0; i < 24; i++)
    {
        ccDrawLine(ccp(0, i*40), ccp(800, i*40));
    }
    for(int i = 0; i < 40; i++)
    {
        ccDrawLine(ccp(i*40, 480), ccp(i*40, 0));
    }
    ccDrawColor4F(0, 0, 0, 0);
}

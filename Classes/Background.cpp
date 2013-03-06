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
    ccDrawLine(ccp(0, 240), ccp(800, 240));
    ccDrawLine(ccp(400, 480), ccp(400, 0));
}

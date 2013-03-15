#include "HelloWorldScene.h"
#include "AppMacros.h"
#include "Lightning.h"
#include "Bomb.h"
#include "Cannon.h"
#include "Background.h"
#include "MapGrid.h"
//#include "Range.h"

USING_NS_CC;


CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    //resourceSize / designSize
    CCDirector::sharedDirector()->setContentScaleFactor(1.0);
    //DesignResolution
    CCDirector::sharedDirector()->getOpenGLView()->setDesignResolutionSize(800, 480, kResolutionExactFit);
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    CCSize winPixle = CCDirector::sharedDirector()->getWinSizeInPixels();

    float contentScalor = CCDirector::sharedDirector()->getContentScaleFactor();

    printf("%f, %f, %f, %f\n", visibleSize.width, visibleSize.height, origin.x, origin.y);
    printf("winSize %f, %f, %f, %f, %f, %f, %f\n", winSize.width, winSize.height, winPixle.width, winPixle.height, contentScalor, visibleSize.width, visibleSize.height);

    
    Lightning *lightning = Lightning::create(NULL, 100, 10.0, 10.0, 20.0);
    lightning->midDisplacement(0, 160, 480, 160, 100.0);
    //lightning->testLine(300 ,400, 50, 200);
    //lightning->testLine(50 ,200, 300, 400);
    this->addChild(lightning);
    //lightning->runAction(CCFadeOut::create(2));

    //Layer3d *layer3d = Layer3d::create();
    //this->addChild(layer3d);
    //Bomb *bomb = Bomb::create();
    //addChild(bomb);

    Background *bk = Background::create();
    addChild(bk);
    
    //Cannon *cannon = Cannon::create();
    //addChild(cannon);
    //cannon->setPosition(ccp(400, 240));

    //Range *range = Range::create();
    //addChild(range);
    
    /*
    CCSprite *sp;
    ccBlendFunc src = {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA};

    sp = CCSprite::create("range_circle.png");
    //sp->setScaleY(0.8);
    sp->setPosition(ccp(50, 50));

    sp->setBlendFunc(src);
    sp->getTexture()->generateMipmap();
    ccTexParams texParams = { GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };    
    sp->getTexture()->setTexParameters(&texParams);

    sp->setScale(0.2);

    addChild(sp);

    sp = CCSprite::create("range_circle.png");
    //sp->setScaleY(0.8);
    sp->setPosition(ccp(100, 100));
    sp->setBlendFunc(src);
    sp->setScale(0.5);
    addChild(sp);

    sp = CCSprite::create("range_circle.png");
    //sp->setScaleY(0.8);
    sp->setPosition(ccp(200, 200));
    sp->setBlendFunc(src);
    sp->setScale(1);
    addChild(sp);

    sp = CCSprite::create("range_circle.png");
    //sp->setScaleY(0.8);
    sp->setPosition(ccp(400, 300));
    sp->setBlendFunc(src);
    sp->setScale(2);
    sp->setOpacity(200); //设定alpha 

    addChild(sp);
    */

    setTouchEnabled(true);

    bid = 0;
    curSize = 1;
    mapGrid = MapGrid::create();
    addChild(mapGrid);

    CCMenuItemFont *item1 = CCMenuItemFont::create("1", this, menu_selector(HelloWorld::on1));
    CCMenuItemFont *item2 = CCMenuItemFont::create("2", this, menu_selector(HelloWorld::on2));
    CCMenuItemFont *item3 = CCMenuItemFont::create("remove", this, menu_selector(HelloWorld::onRemove));
    CCMenu *menu = CCMenu::create(item1, item2, item3,NULL);
    addChild(menu);
    menu->setPosition(ccp(0, 0));
    item1->setPosition(ccp(700, 100));
    item2->setPosition(ccp(700, 200));
    item3->setPosition(ccp(700, 300));
    return true;
}
void HelloWorld::on1() {
    curSize = 1;
}
void HelloWorld::on2() {
    curSize = 2;
}
void HelloWorld::onRemove() {
    mapGrid->clearBuilding(--bid);
}

bool HelloWorld::ccTouchBegan(CCTouch *touch, CCEvent *event) {
    return true;
}
void HelloWorld::ccTouchMoved(CCTouch *touch, CCEvent *event) {
    //CCPoint touchPoint = touch->getLocation();

}
void HelloWorld::registerWithTouchDispatcher() {
    CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
    pDispatcher->addTargetedDelegate(this, 0, true);
}
void HelloWorld::ccTouchEnded(CCTouch *touch, CCEvent *event) {
    CCPoint touchPoint = touch->getLocation();
    CCLog("touchPoint %f %f\n", touchPoint.x, touchPoint.y);

    int xIndex = touchPoint.x/(mapGrid->cellWidth/2);
    int yIndex = touchPoint.y/(mapGrid->cellHeight/2);

    int initX = xIndex+curSize-1;
    int initY = yIndex;

    //调整位置 使网格 片的 最下面一行的面片的左下角 奇数偶数 性相同
    if(initX%2 != initY%2)
        yIndex++;


    mapGrid->putBuilding(bid++, xIndex, yIndex, curSize, curSize);
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

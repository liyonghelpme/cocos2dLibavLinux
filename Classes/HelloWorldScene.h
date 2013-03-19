#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "VideoController.h"
#include "cocos2d.h"
#include "MapGrid.h"

class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);

    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(CCTouch *touch, CCEvent *event);

    virtual void registerWithTouchDispatcher();

private:
    MapGrid *mapGrid;
    int bid;
    int curSize;

    void on1();
    void on2();
    void onRemove();
    void onStart(CCObject *);
    void onStop(CCObject *);
    CCMenu *menu;

    VideoController *video; 
};

#endif // __HELLOWORLD_SCENE_H__

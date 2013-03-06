#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;
class Background : public CCNode {
public:
    static Background *create();
    virtual void draw();
};
#endif

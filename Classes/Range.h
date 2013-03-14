#ifndef __RANGE_H__
#define __RANGE_H__
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

class Range : public  CCNode {
public:
    static Range *create();
    virtual void draw();
    void init();
private:
    float sy;
    float radius;
};

#endif

#ifndef __CANNON_H__
#define __CANNON_H__
#include "cocos2d.h"
#include "Bomb.h"
using namespace cocos2d;
using namespace std;
typedef enum {
    FREE,
    START,
    FLY,
    BOMB,
} CANNON_STATE;

class Cannon : public CCNode {
public:
    Cannon();
    ~Cannon();
    virtual void draw();
    virtual void update(float dt);
    static Cannon *create();
    void onEnter();
private:    
    float dir;
    CANNON_STATE state;
    float passTime;
    Bomb *bomb;
};
#endif

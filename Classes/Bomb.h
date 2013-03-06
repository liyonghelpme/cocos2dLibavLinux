#ifndef __BOMB_H__
#define __BOMB_H__
#include "cocos2d.h"
using namespace cocos2d;
using namespace std;

class Bomb : public CCNode {
public:
    static Bomb *create();
    void init();
    virtual void onEnter();
    virtual void update(float dt);
    void updateQuads(float dt);
    CCParticleSystemQuad *bombStart(CCPoint &pos, float dir, float time);
    CCParticleSystemQuad *bombFly(CCPoint &start, CCPoint &end, float flyTime);
    CCParticleSystemQuad *bombEnd(CCPoint &end);
    void removeLaser();
private:
    CCParticleSystem *system;
    CCParticleSystem *laser;
};
#endif

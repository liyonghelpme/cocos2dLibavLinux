#include "Cannon.h"
#include "math.h"
#include "stdlib.h"
#include "kazmath/utility.h"

Cannon::Cannon():
    dir(0),
    state(FREE)
{
    //bomb = Bomb::create();
    //bomb->retain();
    //addChild(bomb);
}
Cannon::~Cannon() {
    //bomb->release();
}
Cannon *Cannon::create() {
    Cannon *pRet = new Cannon();
    pRet->setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));
    //pRet->setPosition(ccp(400, 240));
    pRet->autorelease();
    return pRet;
}
/*
加农炮的状态更新:
    三个大阶段:
        汇集能量
        发射激光
        爆炸
    每个阶段包含3个小阶段:
        根据passTime 来区分
        阶段开始准备状态  阶段运行计时 阶段结束清理状态

    可以按照堆栈的方式管理状态:
        压入状态End Fly Start  再按照反方向弹出执行阶段

*/
void Cannon::update(float dt) {
    //printf("state %d\n", state, passTime, dir);
    if(state == FREE) {
        float v = rand()*1.0/RAND_MAX;
        dir = v*kmPI*2;
        state = START;
        passTime = 0;
    /*
        开火:
            设定bomb 
    */
    } else if(state == START) {
        //传入位置和其它参数
        if(passTime == 0){
            CCPoint p = getPosition();
            float vx = cos(dir)*15; //炮弹口位置
            float vy = sin(dir)*15; 
            CCPoint d = ccp(p.x+vx, p.y+vy);

            bomb = Bomb::create();
            getParent()->addChild(bomb);
            printf("pos, dir %lf, %lf, %lf\n", d.x, d.y, dir);
            bomb->bombStart(d, dir, 1.0);//位置 朝向 时间
            
            passTime += dt; 
        } else if(passTime < 1.0) {
            passTime += dt;
        } else if(passTime >= 1.0) {
            state = FLY;
            passTime = 0;
        }
    } else if(state == FLY) {
        if(passTime == 0) {
            CCPoint p = getPosition();
            float vx = cos(dir); //炮弹口位置
            float vy = sin(dir); 
            CCPoint d = ccp(p.x+vx*15, p.y+vy*15);
            CCPoint e = ccp(p.x+vx*200, p.y+vy*200);

            bomb->bombFly(d, e, 2);//从起点 飞到目的点 飞行时间 飞行方向 
            passTime += dt; 
        } else if(passTime < 2.0) { 
            passTime += dt;
        } else if(passTime >= 2.0) {//飞行阶段
            state = BOMB;
            passTime = 0;
        }
    /*
        爆炸结束 移除bomb
    */
    } else if(state == BOMB) { //爆炸阶段
        if(passTime == 0) {
            CCPoint p = getPosition();//攻击中点
            float vx = cos(dir); //炮弹口位置
            float vy = sin(dir); 
            CCPoint e = ccp(p.x+vx*200, p.y+vy*200);

            bomb->bombEnd(e);
            passTime += dt; 
        } else if(passTime < 1.0) {
            passTime += dt;
        } else if(passTime >= 1.0) {
            state = FREE;
            passTime = 0;
            bomb->removeFromParent();
            bomb = NULL;
        }
    }
}
//如何旋转只转动炮口而已
void Cannon::draw() {
    CC_NODE_DRAW_SETUP();
    ccDrawSolidRect(ccp(-10, -10), ccp(10, 10), ccc4f(0.8, 0.8, 0.8, 1));
    ccDrawColor4F(1.0, 0.2, 0.3, 1);
    
    float vx = cos(dir)*15;
    float vy = sin(dir)*15;
    CCPoint d = ccp(vx, vy);

    ccDrawLine(ccp(0, 0), d);
    
    ccDrawColor4F(0, 0, 0, 0);

}
void Cannon::onEnter() {
    CCNode::onEnter();
    scheduleUpdate();
}

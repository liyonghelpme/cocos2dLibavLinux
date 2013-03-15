#include "MapGrid.h"
#include "kazmath/utility.h"

MapGrid *MapGrid::create()
{
    MapGrid *pRet = new MapGrid();
    pRet->init();
    pRet->autorelease();
    return pRet;
}
//object new referenceNum = 1
void MapGrid::init()
{
    cellWidth = 80;
    cellHeight = 80;
    cellNum = 20;
    lineWidth = 10;
    //lineLength = 100;
    lineColor = ccc3(255, 10, 10);

    buildings = new CCDictionary();
    blocks = new CCDictionary();
    edges = new CCDictionary();

    ccBlendFunc blendFunc = {GL_ONE, GL_ONE};
    blockBatch = CCSpriteBatchNode::create("block.png", 100);
    blockBatch->setBlendFunc(blendFunc);

    lineBatch = CCSpriteBatchNode::create("edge.png", 100);
    lineBatch->setBlendFunc(blendFunc);

    addChild(blockBatch);
    addChild(lineBatch);

    //addLine(0, 0, 5, 5);
}
int MapGrid::getKey(int x, int y)
{
    return x*10000+y;
}

void MapGrid::getXY(int key, int &x, int &y)
{
    x = key/10000;
    y = key%10000;
}

//building 建筑物
//addObject 每个网格的建筑物
//batch addChild

//孩子节点使用 tag 作为block的标识
//计算线的位置
//x0 y0 x1 y1*10 0 1 2 不要超出正整数的边界否则需要用string 来做key 用于删除childNode
//每个块对应4条边 块的数组
//key ---> edge0 edge1 edge2 edge3   tag 编号 
// 某些块 有引入边
void MapGrid::removeLine(int blockKey, int edgeId) {

    int realEdgeId = blockKey*10+edgeId;
    CCLog("removeLine %d", realEdgeId);
    CCArray *arr = (CCArray *)edges->objectForKey(realEdgeId);

    CCObject* child;
    CCARRAY_FOREACH(arr, child) {
        ((CCNode*)child)->removeFromParent();
    }

    edges->removeObjectForKey(realEdgeId);
}
void MapGrid::addLine(int blockKey, int x0, int y0, int x1, int y1, int edgeId) {
    float vx0 = x0*cellWidth/2;
    float vy0 = y0*cellHeight/2;
    float vx1 = x1*cellWidth/2;
    float vy1 = y1*cellHeight/2;

    kmVec3 a, b;
    kmVec3Fill(&a, vx0, vy0, 0);
    kmVec3Fill(&b, vx1, vy1, 0);
    
    kmVec3 temp;
    kmVec3Subtract(&temp, &b, &a);
    double deg = -atan2(temp.y, temp.x)*180/kmPI;
    
    int realEdgeId = blockKey*10+edgeId;
    CCLog("addLine %d", realEdgeId);

    CCArray *arr = CCArray::create();
    edges->setObject(arr, realEdgeId);


    CCSprite *line = CCSprite::create("edge.png");
    line->setTextureRect(CCRectMake(63, 0, 1, 128));//宽第1的中间像素
    line->setAnchorPoint(ccp(0, 0.5));
    line->setScaleX(kmVec3Length(&temp)/1.0);
    line->setScaleY(lineWidth/128);
    line->setPosition(ccp(vx0, vy0));
    line->setRotation(deg);
    line->setColor(lineColor);
    lineBatch->addChild(line, 0, realEdgeId);
    arr->addObject(line);

    CCSprite *s = CCSprite::create("edge.png"); 
    s->setAnchorPoint(ccp(1.0, 0.5));
    s->setPosition(ccp(vx0, vy0));
    s->setRotation(deg);
    s->setScale(lineWidth/128);
    s->setColor(lineColor);
    lineBatch->addChild(s, 0, realEdgeId);
    arr->addObject(s);

    s = CCSprite::create("edge.png");
    s->setAnchorPoint(ccp(0, 0.5));
    s->setPosition(ccp(vx1, vy1));
    s->setRotation(deg);
    s->setFlipX(true);
    s->setScale(lineWidth/128);
    s->setColor(lineColor);
    lineBatch->addChild(s, 0, realEdgeId);
    arr->addObject(s);
}
//新占领的块需要调整边线显示
void MapGrid::putBuilding(int bid, int x, int y, int ex, int ey)
{
    CCLog("putBuilding %d %d %d %d %d", bid, x, y, ex, ey);
    BuildingElement *be = new BuildingElement(bid, x, y, ex, ey);
    buildings->setObject(be, bid);
    be->release();
    
    
    for(int i = 0; i < ex; i++) {
        int initX = x+ex-i-1;
        int initY = y+i;
        for(int j = 0; j < ey; j++) {
            int key = getKey(initX, initY);
            CCArray *arr = (CCArray*)blocks->objectForKey(key); 
            CCLOG("add Object %d %d %x", i, j, arr);
            //该网格的第一个建筑物
            if(arr == NULL) {
                arr = CCArray::create();
                blocks->setObject(arr, key);

                CCSprite *b = CCSprite::create("block.png");
                float left = initX*cellWidth/2;
                float bottom = initY*cellHeight/2;

                b->setPosition(ccp(left, bottom));
                CCSize csize = b->getContentSize();
                b->setScaleX(cellWidth/csize.width);
                b->setScaleY(cellHeight/csize.height);
                b->setAnchorPoint(ccp(0, 0));

                blockBatch->addChild(b, 0, key);

                //检测网格的边是否显示
                int key0 = getKey(initX-1, initY+1);
                if(blocks->objectForKey(key0) == NULL) {
                    addLine(key, initX, initY+1, initX+1, initY+2, 0);
                } else {
                    int otherId = 2;
                    removeLine(key0, otherId);
                }
                key0 = getKey(initX+1, initY+1);
                if(blocks->objectForKey(key0) == NULL) {
                    addLine(key, initX+1, initY+2, initX+2, initY+1, 1);
                } else {
                    int otherId = 3;
                    removeLine(key0, otherId);
                }
                key0 = getKey(initX+1, initY-1);
                if(blocks->objectForKey(key0) == NULL) {
                    addLine(key, initX+2, initY+1, initX+1, initY, 2);
                } else {
                    int otherId = 0;
                    removeLine(key0, otherId);
                }
                key0 = getKey(initX-1, initY-1);
                if(blocks->objectForKey(key0) == NULL) {
                    addLine(key, initX+1, initY, initX, initY+1, 3);
                } else {
                    int otherId = 1;
                    removeLine(key0, otherId);
                }
            }
            arr->addObject(be);
            initX += 1;
            initY += 1;
        }
    }
}
void MapGrid::clearBuilding(int bid)
{
    BuildingElement *be = (BuildingElement*)(buildings->objectForKey(bid));
    int x, y, ex, ey;
    x = be->x;
    y = be->y;
    ex = be->ex;
    ey = be->ey;

    for(int i = 0; i < ex; i++) {
        int initX = x+ex-i-1;
        int initY = y+i;

        for(int j = 0; j < ey; j++) {
            int key = getKey(initX, initY);
            CCArray *arr = (CCArray *)blocks->objectForKey(key); 
            //该网格的最后一个建筑物
            if(arr != NULL) {
                arr->removeObject(be);
                //移除块 也要增加 或者 移除边
                if(arr->count() == 0) {
                    blocks->removeObjectForKey(key);
                    blockBatch->removeChildByTag(key);

                    int key0 = getKey(initX-1, initY+1);
                    if(blocks->objectForKey(key0) != NULL) {
                        int otherId = 2;
                        addLine(key0, initX, initY+1, initX+1, initY+2, otherId);
                    } else {
                        removeLine(key, 0);
                    }
                    key0 = getKey(initX+1, initY+1);
                    if(blocks->objectForKey(key0) != NULL) {
                        int otherId = 3;
                        addLine(key0, initX+1, initY+2, initX+2, initY+1, otherId);
                    } else {
                        removeLine(key, 1);
                    }
                    key0 = getKey(initX+1, initY-1);
                    if(blocks->objectForKey(key0) != NULL) {
                        int otherId = 0;
                        addLine(key0, initX+2, initY+1, initX+1, initY, otherId);
                    } else {
                        removeLine(key, 2);
                    }
                    key0 = getKey(initX-1, initY-1);
                    if(blocks->objectForKey(key0) != NULL) {
                        int otherId = 1;
                        addLine(key0, initX+1, initY, initX, initY+1, otherId);
                    } else {
                        removeLine(key, 3);
                    }

                }
            }

            initX += 1;
            initY += 1;
        }
    }

    //释放 delete 在之前的操作中需要保持引用
    buildings->removeObjectForKey(bid);
}

MapGrid::~MapGrid()
{
    buildings->release();
    blocks->release();
    edges->release();
}

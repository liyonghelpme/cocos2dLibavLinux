#ifndef __MAP_GRID_H__
#define __MAP_GRID_H__
#include "cocos2d.h"
using namespace cocos2d;
class BuildingElement : public CCObject {
public:
    int bid;
    int x, y, ex, ey;
    BuildingElement(int i, int a, int b, int c, int d)
    :bid(i), x(a), y(b), ex(c), ey(d)
    {
    }
};

class MapGrid : public CCNode{
public:
    static MapGrid *create();
    void init();
    ~MapGrid();
    void putBuilding(int bid, int x, int y, int ex, int ey);
    void clearBuilding(int bid);

    float cellWidth;
    float cellHeight;
    int cellNum;

private:
    CCSpriteBatchNode *blockBatch;
    CCSpriteBatchNode *lineBatch;
    CCDictionary *buildings;//bid --> array
    CCDictionary *blocks;// grid 
    CCDictionary *edges;


    int getKey(int x, int y);
    void getXY(int key, int &x, int &y);
    //线的tag 也是 网格的编号
    void addLine(int key, int x0, int y0, int x1, int y1, int edgeId);
    void removeLine(int blockKey, int edgeId);

    float lineWidth;
    float lineLength;
    ccColor3B lineColor;
};
#endif

#ifndef __MAP_GRID_H__
#define __MAP_GRID_H__
#include "cocos2d.h"
using namespace cocos2d;

//建筑物的影响范围 数据结构 应该作为一个组件组合到 建筑物类上
class BuildingElement : public CCObject {
public:
    int bid;
    int x, y, ex, ey;
    BuildingElement(int i, int a, int b, int c, int d)
    :bid(i), x(a), y(b), ex(c), ey(d)
    {
    }
    static BuildingElement *create(int i, int a, int b, int c, int d) {
        BuildingElement *build = new BuildingElement(i, a, b, c, d);
        build->autorelease();
        return build;
    }
};


/*
0，0 点位置开始的网格
首先绘制Block 接着绘制Line
需要设定
    网格的大小
    线的颜色
    Block的使用的纹理
    Line使用的纹理

MapGrid *mapGrid = MapGrid::create();
mapGrid->putBuilding(BuildingElement);
mapGrid->clearBuilding(bid);//或者 使用BuildingElement？
*/
class MapGrid : public CCNode{
public:
    static MapGrid *create();
    void init();
    ~MapGrid();
    /*
        放置建筑物 
        参数：
            建筑物结构
    */
    void putBuilding(BuildingElement *build);
    /*
        移除建筑物
        参数：
            建筑物id
    */
    void clearBuilding(int bid);

    float cellWidth; //网格实际宽度  
    float cellHeight; //网格的实际高度
    int cellNum; 

private:
    //绘制白色块的batch
    CCSpriteBatchNode *blockBatch;
    //绘制边缘线的batch
    CCSpriteBatchNode *lineBatch;
    
    //建筑物列表 根据建筑物id 得到建筑物
    CCDictionary *buildings;//bid --> array
    
    //块列表 根据块id 指向一组建筑物，因为建筑物的影响范围可能会重叠
    CCDictionary *blocks;// grid 
    
    //边缘线列表 根据 块id 和 边的编号(0, 1, 2, 3) 获取边
    CCDictionary *edges;

    //块位置 和 块id 互相转化的函数
    int getKey(int x, int y);
    void getXY(int key, int &x, int &y);

    //增加边缘线 和删除边缘线的函数
    void addLine(int key, int x0, int y0, int x1, int y1, int edgeId);
    void removeLine(int blockKey, int edgeId);

    //边缘线的宽度
    float lineWidth;
    //float lineLength;
    //边缘线的颜色
    ccColor3B lineColor;
};
#endif

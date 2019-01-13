#ifndef GVECTOR3_H
#define GVECTOR3_H

#include <iostream>
#include <cmath>
#include <algorithm>
class GVector3 {
public:
    float x;
    float y;
    float z;
     // 缺省构造函数
    GVector3() {};
    // 用户构造函数
    GVector3(float posX, float posY, float posZ) :x(posX), y(posY), z(posZ) {}
    ~GVector3() {};
    //输出向量信息
    void getInfo();
    //矢量加法
    GVector3 operator+(const GVector3 &v);
    //矢量减法
    GVector3 operator-(const GVector3 &v);
    //数乘
    GVector3 operator*(const float &n);
    //数除
    GVector3 operator/(const float &n);
    //向量点积
    float dotMul(GVector3 v2);
    //向量叉乘
    GVector3 crossMul(GVector3 v2);
    //绝对值化
    GVector3 abs();
    //获取分量中的最大值
    float max();
    //获取分量的最小值
    float min();
    //获取矢量长度
    float getLength();
    //向量单位化
    GVector3 normalize();
    //向量取反
    GVector3 negate();
    //求两点之间的距离
    float getDist(GVector3 v);
    //返回零向量
    static inline GVector3 zero(){ return GVector3(0,0,0); }
    //打印向量的分量值
    void show();
};
 
#endif // GVECTOR3_H
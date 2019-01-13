#include "gvector3.h"


GVector3 GVector3::operator+(const GVector3 &v)
{
    return GVector3(x+v.x,v.y+y,v.z+z);
}
GVector3 GVector3::operator-(const GVector3 &v)
{
    return GVector3(x-v.x,y-v.y,z-v.z);
}
GVector3 GVector3::operator*(const float &n)
{
    return GVector3(x*n,y*n,z*n);
}
GVector3 GVector3::operator/(const float &n)
{
    return GVector3(x/n,y/n,z/n);
}
void GVector3::getInfo()
{
    std::cout<<"x:"<<x<<" y:"<<y<<" z:"<<z<< std::endl;
}
GVector3 GVector3::abs()
{
    if(x<0) x*=-1;
    if(y<0) y*=-1;
    if(z<0) z*=-1;
    return GVector3(x,y,z);
}
float GVector3::dotMul(GVector3 v2)
{
    return (x*v2.x+y*v2.y+z*v2.z);
}
GVector3 GVector3::crossMul(GVector3 v2)
{
    GVector3 vNormal;
    // 计算垂直矢量
    vNormal.x = ((y * v2.z) - (z * v2.y));
    vNormal.y = ((z * v2.x) - (x * v2.z));
    vNormal.z = ((x * v2.y) - (y * v2.x));
    return vNormal;
}
float GVector3::getLength()
{
    return  (float)sqrt(x*x+y*y+z*z);
}
GVector3 GVector3::normalize()
{
    float length=getLength();
    if (length == 0)
        return GVector3(0, 0, 0);
    x=x/length;
    y=y/length;
    z=z/length;
    return GVector3(x,y,z);
}
GVector3 GVector3::negate()
{
    return GVector3(-x,-y,-z);
}
void GVector3::show()
{
    std::cout<<"x:"<<x<<"  y:"<<y<<"  z"<<z<<std::endl;
}
float GVector3::max()
{
    float tmp=std::max(y,z);
    return std::max(x,tmp);
}
float GVector3::min()
{
    float tmp=std::min(y,z);
    return std::min(x,tmp);
}
float GVector3::getDist(GVector3 v)
{
    float tmp=(x-v.x)*(x-v.x)+(y-v.y)*(y-v.y)+(z-v.z)*(z-v.z);
    return sqrt(tmp);
}
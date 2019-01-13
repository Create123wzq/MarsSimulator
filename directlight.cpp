#include "directlight.h"

DirectLight::DirectLight()
{
    //ctor
}
DirectLight::DirectLight(Color _color,GVector3 _direction,bool _isShadow)
{
    color=_color;
    direction=_direction;
    isShadow=_isShadow;
}
DirectLight::~DirectLight()
{
    //dtor
}
//通过光线与场景的相交结果计算光照结果
Color DirectLight::intersect(CSphere &sphereModel, IntersectResult &rayResult, float* rgb)
{
    //生产shadowRay的修正值
    const float k=1e-4;
    //生成与光照相反方向的shadowRay
    GVector3 shadowDir=direction.normalize().negate();
    CRay shadowRay=CRay(rayResult.position+rayResult.normal*k,shadowDir);
    //计算shadowRay是否与场景相交
    IntersectResult lightResult = sphereModel.isIntersected(shadowRay);
    Color resultColor = Color(rgb[0]/255, rgb[1]/255, rgb[2]/255);
    if(isShadow)
    {
        if(lightResult.isHit==1)
        {
            return Color::black();
        }
    }

    //计算光强
    float NdotL=rayResult.normal.dotMul(shadowDir);
    //if (NdotL >= 0)
        resultColor=resultColor.add(this->color.multiply(NdotL));
    //return this->color;

    return resultColor;
}

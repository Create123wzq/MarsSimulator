#ifndef DIRECTLIGHT_H
#define DIRECTLIGHT_H
#include "color.h"
#include "gvector3.h"
#include "csphere.h"

class DirectLight
{
public:
    DirectLight();
    DirectLight(Color _color,GVector3 _direction,bool _isShadow);
    virtual ~DirectLight();
    Color intersect(CSphere &sphereModel, IntersectResult &result, float* rgb);
protected:
private:
    bool isShadow;
    Color color;
    GVector3 direction;
};
#endif // DIRECTLIGHT_H

#include "gluetagtoplane.h"

GluetagToPlane::GluetagToPlane(Gluetag& gluetag) : _gluetag(gluetag)
{
}

GluetagToPlane::~GluetagToPlane()
{

}

QVector2D const& GluetagToPlane::get(QVector3D const &vec)
{
    if(vec == _gluetag._bl)
        return a;
    if(vec == _gluetag._br)
        return b;
    if(vec == _gluetag._tl)
        return c;
    if(vec == _gluetag._tr)
        return d;
    throw std::invalid_argument("something went wrong trying to retrieve 2D representation");
}

QVector3D const& GluetagToPlane::get(QVector2D const &vec)
{
    if(vec == a)
        return _gluetag._bl;
    if(vec == b)
        return _gluetag._br;
    if(vec == c)
        return _gluetag._tl;
    if(vec == d)
        return _gluetag._tr;

    throw std::invalid_argument("something went wrong trying to retrieve 3D representation");
}

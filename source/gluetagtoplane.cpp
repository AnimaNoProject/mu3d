#include "gluetagtoplane.h"

QVector3D GluetagToPlane::_colorOverlap = QVector3D(0.9f, 0.0f, 0.0f);
QVector3D GluetagToPlane::_color = QVector3D(0.0f,0.5f,1.0f);

GluetagToPlane::GluetagToPlane(Gluetag* gluetag) : _gluetag(gluetag)
{
}

GluetagToPlane::~GluetagToPlane()
{

}

void GluetagToPlane::drawproperties(std::vector<QVector3D>& vertices, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colors)
{
    vertices.push_back(QVector3D(a, 0));
    vertices.push_back(QVector3D(b, 0));
    vertices.push_back(QVector3D(c, 0));

    vertices.push_back(QVector3D(c, 0));
    vertices.push_back(QVector3D(b, 0));
    vertices.push_back(QVector3D(d, 0));

    verticesLines.push_back(QVector3D(a, 0));
    verticesLines.push_back(QVector3D(c, 0));

    verticesLines.push_back(QVector3D(b, 0));
    verticesLines.push_back(QVector3D(d, 0));

    verticesLines.push_back(QVector3D(d, 0));
    verticesLines.push_back(QVector3D(c, 0));

    // color
    if(_overlaps)
    {
        colors.push_back(_colorOverlap);
        colors.push_back(_colorOverlap);
        colors.push_back(_colorOverlap);
        colors.push_back(_colorOverlap);
        colors.push_back(_colorOverlap);
        colors.push_back(_colorOverlap);
    }
    else
    {
        colors.push_back(_gluetag->_color);
        colors.push_back(_gluetag->_color);
        colors.push_back(_gluetag->_color);
        colors.push_back(_gluetag->_color);
        colors.push_back(_gluetag->_color);
        colors.push_back(_gluetag->_color);
    }
}

double GluetagToPlane::overlaps(GluetagToPlane& other)
{
    if( Utility::intersects(a, c, other.a, other.c)
        || Utility::intersects(a, c, other.c, other.d)
        || Utility::intersects(a, c, other.b, other.d)

        || Utility::intersects(c, d, other.a, other.c)
        || Utility::intersects(c, d, other.c, other.d)
        || Utility::intersects(c, d, other.b, other.d)

        || Utility::intersects(b, d, other.a, other.c)
        || Utility::intersects(b, d, other.c, other.d)
        || Utility::intersects(b, d, other.b, other.d))
    {
        return Utility::intersectionArea(a, b, c, other.a, other.b, other.c) + Utility::intersectionArea(c, b, d, other.a, other.b, other.c) +
            Utility::intersectionArea(a, b, c, other.c, other.b, other.d) + Utility::intersectionArea(c, b, d, other.c, other.b, other.d);
    }
    else
    {
        return 0;
    }
}

double GluetagToPlane::overlaps(FaceToPlane& other)
{
    if ( Utility::intersects(a, c, other.a, other.b)
        || Utility::intersects(a, c, other.b, other.c)
        || Utility::intersects(a, c, other.c, other.a)

        || Utility::intersects(c, d, other.a, other.b)
        || Utility::intersects(c, d, other.b, other.c)
        || Utility::intersects(c, d, other.c, other.a)

        || Utility::intersects(b, d, other.a, other.b)
        || Utility::intersects(b, d, other.b, other.c)
        || Utility::intersects(b, d, other.c, other.a))
    {
        return Utility::intersectionArea(a, b, c, other.a, other.b, other.c) + Utility::intersectionArea(c, b, d, other.a, other.b, other.c);
    }
    else
    {
        return 0;
    }

}

QVector2D const& GluetagToPlane::get(QVector3D const &vec)
{
    if(vec == _gluetag->_bl)
        return a;
    if(vec == _gluetag->_br)
        return b;
    if(vec == _gluetag->_tl)
        return c;
    if(vec == _gluetag->_tr)
        return d;
    throw std::invalid_argument("something went wrong trying to retrieve 2D representation");
}

QVector3D const& GluetagToPlane::get(QVector2D const &vec)
{
    if(vec == a)
        return _gluetag->_bl;
    if(vec == b)
        return _gluetag->_br;
    if(vec == c)
        return _gluetag->_tl;
    if(vec == d)
        return _gluetag->_tr;

    throw std::invalid_argument("something went wrong trying to retrieve 3D representation");
}

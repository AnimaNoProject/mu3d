#include "facetoplane.h"

QVector3D FaceToPlane::_color = QVector3D(1.0f, 1.0f, 1.0f);
QVector3D FaceToPlane::_colorOverlap = QVector3D(0.9f, 0.0f, 0.0f);

FaceToPlane::~FaceToPlane()
{

}

FaceToPlane::FaceToPlane() : _overlaps(false)
{
}

double FaceToPlane::overlaps(FaceToPlane& other)
{
    if(Utility::intersects(a, b, other.a, other.b) ||
       Utility::intersects(a, b, other.b, other.c) ||
       Utility::intersects(a, b, other.c, other.a) ||
       Utility::intersects(b, c, other.a, other.b) ||
       Utility::intersects(b, c, other.b, other.c) ||
       Utility::intersects(b, c, other.c, other.a) ||
       Utility::intersects(c, a, other.a, other.b) ||
       Utility::intersects(c, a, other.b, other.c) ||
       Utility::intersects(c, a, other.c, other.a))
    {
        return Utility::intersectionArea(a, b, c, other.a, other.b, other.c);
    }
    else
    {
        return 0;
    }

}

void FaceToPlane::drawproperties(std::vector<QVector3D>& vertices, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colors)
{
    vertices.push_back(QVector3D(a, 0));
    vertices.push_back(QVector3D(b, 0));
    vertices.push_back(QVector3D(c, 0));

    verticesLines.push_back(QVector3D(a, 0));
    verticesLines.push_back(QVector3D(b, 0));

    verticesLines.push_back(QVector3D(b, 0));
    verticesLines.push_back(QVector3D(c, 0));

    verticesLines.push_back(QVector3D(a, 0));
    verticesLines.push_back(QVector3D(c, 0));

    if(_overlaps)
    {
        colors.push_back(_colorOverlap);
        colors.push_back(_colorOverlap);
        colors.push_back(_colorOverlap);
    }
    else
    {
        colors.push_back(_color);
        colors.push_back(_color);
        colors.push_back(_color);
    }
}

QVector2D const& FaceToPlane::get(QVector3D const &vec)
{
    if(vec == A)
        return a;
    if(vec == B)
        return b;
    if(vec == C)
        return c;
    else
        throw std::invalid_argument("something went wrong trying to retrieve 2D representation");
}

QVector3D const& FaceToPlane::get(QVector2D const &vec)
{
    if(vec == a)
        return A;
    if(vec == b)
        return B;
    if(vec == c)
        return C;
    else
        throw std::invalid_argument("something went wrong trying to retrieve 3D representation");
}

QVector3D const& FaceToPlane::get(QVector3D const &one, QVector3D const &two)
{
    if(one != A && two != A)
        return A;
    if(one != B && two != B)
        return B;
    if(one != C && two != C)
        return C;
    else
        throw std::invalid_argument("something went wrong");

}

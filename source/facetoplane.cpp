#include "facetoplane.h"

bool FaceToPlane::overlaps(FaceToPlane& other)
{
    return Utility::intersects(a, b, other.a, other.b)
           || Utility::intersects(a, b, other.b, other.c)
           || Utility::intersects(a, b, other.c, other.a)
           || Utility::intersects(b, c, other.a, other.b)
           || Utility::intersects(b, c, other.b, other.c)
           || Utility::intersects(b, c, other.c, other.a)
           || Utility::intersects(c, a, other.a, other.b)
           || Utility::intersects(c, a, other.b, other.c)
           || Utility::intersects(c, a, other.c, other.a);
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

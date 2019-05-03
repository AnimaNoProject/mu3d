#include "facetoplane.h"

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

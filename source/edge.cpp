#include "edge.h"

Edge::Edge(int sFace, int tFace, double distance, QVector3D middle)
{
    _sFace = sFace;
    _tFace = tFace;
    _distance = distance;
    _middle = middle;
}

bool Edge::operator==(const Edge& other) const
{
    if((_sFace == other._sFace && _tFace == other._tFace) ||
        (_sFace == other._tFace && _tFace == other._sFace))
        return true;

    return false;
}

bool Edge::operator<(const Edge& other) const
{
    return _distance < other._distance;
}

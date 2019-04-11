#include "edge.h"

Edge::Edge(int sFace, int tFace, double distance, QVector3D middle, Halfedge halfedge, Facet sFacetHandle, Facet tFacetHandle)
{
    _sFace = sFace;
    _tFace = tFace;
    _distance = distance;
    _middle = middle;
    _halfedge = halfedge;
    _sFacetHandle = sFacetHandle;
    _tFacetHandle = tFacetHandle;
}

Edge::Edge()
{

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

Facet Edge::operator[] (int i) const
{
    if(i == _sFace)
        return _sFacetHandle;
    else if (i == _tFace)
        return _tFacetHandle;
    else
        throw std::invalid_argument("received value matching neither source face id, nor target face id");
}

#include "edge.h"

Edge::Edge(int sFace, int tFace, double length, QVector3D middle, Halfedge halfedge, Facet sFacetHandle, Facet tFacetHandle)
    :  _sFace(sFace), _tFace(tFace),
      _sFacetHandle(sFacetHandle), _tFacetHandle(tFacetHandle),
      _length(length),  _halfedge(halfedge),
      _middle(middle),  _weight(length)
{
}

Edge::Edge(int sFace, int tFace)
    : _sFace(sFace), _tFace(tFace)
{
}

Edge::Edge()
{

}

bool Edge::isNeighbour(const Edge& edge)
{
    return _halfedge->vertex() == edge._halfedge->vertex()
        || _halfedge->prev()->vertex() == edge._halfedge->vertex()
        || _halfedge->vertex() == edge._halfedge->prev()->vertex()
        || _halfedge->prev()->vertex() == edge._halfedge->prev()->vertex();
}

bool Edge::is(int A, int B)
{
    return (_sFace == A && _tFace == B) || (_sFace == B && _tFace == A);
}

bool Edge::operator==(const Edge& other) const
{
    return (_sFace == other._sFace && _tFace == other._tFace) ||
        (_sFace == other._tFace && _tFace == other._sFace);
}

bool Edge::operator<(const Edge& other) const
{
    return _weight < other._weight;
}

Facet Edge::operator[] (int i) const
{
    if(i == _sFace)
        return _sFacetHandle;
    else if (i == _tFace)
        return _tFacetHandle;
    else
        throw std::invalid_argument("parameter value matches neither source face id, nor target face id");
}

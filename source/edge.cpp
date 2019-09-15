#include "edge.h"

Edge::Edge(int sFace, int tFace, QVector3D middle, Halfedge halfedge, Facet sFacetHandle, Facet tFacetHandle)
    :  _sFace(sFace), _tFace(tFace),
      _sFacetHandle(sFacetHandle), _tFacetHandle(tFacetHandle),  _halfedge(halfedge),
      _middle(middle),  _weight(double(std::rand()) / RAND_MAX)
{

    /*
     * Calculate if the edge is bent inwards or outwards.
     * */
    isInwards = true;

    QVector3D A = pointToVector(sFacetHandle->halfedge()->vertex()->point());
    QVector3D B = pointToVector(sFacetHandle->halfedge()->next()->vertex()->point());
    QVector3D C = pointToVector(sFacetHandle->halfedge()->next()->next()->vertex()->point());

    QVector3D F = pointToVector(tFacetHandle->halfedge()->vertex()->point());
    QVector3D G = pointToVector(tFacetHandle->halfedge()->next()->vertex()->point());
    QVector3D H = pointToVector(tFacetHandle->halfedge()->next()->next()->vertex()->point());

    QVector3D NABC = QVector3D::crossProduct(B-A, C-A);
    QVector3D NFGH = QVector3D::crossProduct(G - F, H - F);

    NABC.normalize();
    NFGH.normalize();

    float angle = std::acos(QVector3D::dotProduct(NABC, NFGH) / (NABC.length() * NFGH.length()));

    std::cout << angle << std::endl;


    if (angle > 180)
    {
        isInwards = false;
    }
}

QVector3D Edge::pointToVector(CGAL::Point_3<CGAL::Simple_cartesian<double>>& point)
{
    return QVector3D(float(point.x()), float(point.y()), float(point.z()));
}

Edge::Edge(int sFace, int tFace)
    : _sFace(sFace), _tFace(tFace)
{
}

Edge::Edge()
{

}

Edge::~Edge()
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

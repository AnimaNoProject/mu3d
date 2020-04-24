#pragma once
#include <QVector3D>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>
#include <stdlib.h>
#include <math.h>


typedef CGAL::Polyhedron_3<CGAL::Simple_cartesian<double>>::Halfedge_handle Halfedge;
typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Polyhedron::Facet_handle Facet;

class Edge
{
public:
    int _sFace;
    int _tFace;
    Facet _sFacetHandle;
    Facet _tFacetHandle;
    Halfedge _halfedge;
    QVector3D _middle;

    double _weight;

    Edge(int sFace, int tFace, QVector3D middle, Halfedge halfedge, Facet sFacetHandle, Facet tFacetHandle);
    Edge(int sFace, int tFace);
    Edge();
    ~Edge();

    bool isInwards;

    bool isNeighbour(const Edge& edge);
    bool is(int A, int B);
    bool operator==(const Edge& other) const;
    bool operator<(const Edge& other) const;
private:
    QVector3D pointToVector(CGAL::Point_3<CGAL::Simple_cartesian<double>>& point);
};

#pragma once
#include <QVector3D>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

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
    double _length;
    Halfedge _halfedge;
    QVector3D _middle;
    double _heat;

    Edge(int sFace, int tFace, double length, QVector3D middle, Halfedge halfedge, Facet sFacetHandle, Facet tFacetHandle);
    Edge(int sFace, int tFace);
    Edge();

    bool operator==(const Edge& other) const;
    bool operator<(const Edge& other) const;
    Facet operator[] (int i) const;
};

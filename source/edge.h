#pragma once
#include <QVector3D>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

typedef CGAL::Polyhedron_3<CGAL::Simple_cartesian<double>>::Halfedge_handle Halfedge;

class Edge
{
public:
    int _sFace;
    int _tFace;
    double _distance;
    Halfedge _halfedge;

    QVector3D _middle;

    Edge(int sFace, int tFace, double distance, QVector3D middle, Halfedge halfedge);
    Edge();

    bool operator==(const Edge& other) const;
    bool operator<(const Edge& other) const;
};

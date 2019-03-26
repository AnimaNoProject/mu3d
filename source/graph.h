#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

#include <map>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;

typedef Polyhedron::Facet_handle Facet;

struct UndirectedEdge
{
    int _sFace;
    int _tFace;

    UndirectedEdge(int sFace, int tFace)
    {
        _sFace = sFace;
        _tFace = tFace;
    }

    bool operator==(UndirectedEdge other)
    {
        if(_sFace == other._sFace && _tFace == other._tFace)
            return true;
        else if(_sFace == other._tFace && _tFace == other._sFace)
            return true;
        else
            return false;
    }
};

struct DirectedEdge
{
    int _sFace;
    int _tFace;

    DirectedEdge(int sFace, int tFace)
    {
        _sFace = sFace;
        _tFace = tFace;
    }

    bool operator==(DirectedEdge other)
    {
        if(_sFace == other._sFace && _tFace == other._tFace)
            return true;
        else
            return false;
    }
};

class Graph
{
public:
    Graph();
    ~Graph();

    void addFace(Facet facet);
    void calculateDual();
private:
    std::map<int, Facet> _facets;
    std::map<UndirectedEdge, double> _undirectedEdges;
    int getFacetID(Facet facet);
    bool exists(UndirectedEdge edge);
};

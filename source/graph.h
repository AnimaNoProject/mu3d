#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

#include "edge.h"

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;

typedef Polyhedron::Facet_handle Facet;

class Graph
{
public:
    Graph();
    ~Graph();
    void addFace(Facet facet);
    void calculateDual();
    void calculateMSP();
private:
    std::map<int, Facet> _facets;
    std::vector<Edge> _edges;
    std::vector<Edge> _mspEdges;

    int getFacetID(Facet facet);
    bool hasEdge(Edge edge);
    bool isAcyclic(int start);
    bool isSingleComponent();
    bool dfs(std::vector<std::vector<int>> const &graph, int start, std::vector<bool> &discovered, int parent);
};

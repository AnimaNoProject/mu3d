#pragma once
#include "glm.hpp"
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>
#include <stdlib.h>
#include <math.h>

    typedef CGAL::Polyhedron_3<CGAL::Simple_cartesian<double>>::Halfedge_handle Halfedge;
    typedef CGAL::Simple_cartesian<double> Kernel;
    typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
    typedef Polyhedron::Facet_handle Facet;

    class edge
    {
    public:
        int _sFace;
        int _tFace;
        Facet _sFacetHandle;
        Facet _tFacetHandle;
        Halfedge _halfedge;

        double _weight;

        edge(int sFace, int tFace, glm::vec3 middle, Halfedge halfedge, Facet sFacetHandle, Facet tFacetHandle);
        edge(int sFace, int tFace);
        edge();
        ~edge();

        bool isInwards;

        bool isNeighbour(const edge& edge);
        bool is(int A, int B);
        bool operator==(const edge& other) const;
        bool operator<(const edge& other) const;
    };

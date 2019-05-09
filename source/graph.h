#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

#include <QOpenGLFunctions_4_5_Core>
#include <QVector3D>

#include "edge.h"
#include "gluetag.h"
#include "gluetagtoplane.h"
#include "facetoplane.h"

#include "utility.h"

class Gluetag;
class GluetagToPlane;
class FaceToPlane;

class Graph
{
public:
    Graph();
    ~Graph();
    void addFace(Facet facet);
    void calculateDual();
    void calculateMSP();
    void calculateGlueTags(std::vector<QVector3D>& gtVertices, std::vector<GLushort>& gtIndices, std::vector<QVector3D>& gtColors);
    void unfoldGraph(std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colorsLines, QMatrix4x4& center);
    void lines(std::vector<QVector3D>& lineVertices, std::vector<QVector3D>& lineColors);
private:
    std::map<int, Facet> _facets;
    std::vector<Edge> _edges;
    std::vector<Edge> _mspEdges;
    std::vector<Edge> _cutEdges;
    std::vector<Gluetag> _gluetags;
    std::vector<Gluetag> _necessaryGluetags;

    std::vector<std::vector<int>> _tree;

    int getFacetID(Facet facet);
    QVector3D faceCenter(Facet facet);
    bool hasEdge(Edge& edge);
    bool isSingleComponent(std::vector<std::vector<int>>& adjacenceList);
    bool isAcyclic(std::vector<std::vector<int>> const &graph, ulong start, std::vector<bool> &discovered, int parent);
    void treeify(std::vector<std::vector<int>> const &edges, ulong index, std::vector<bool>& discovered, ulong parent, std::vector<FaceToPlane>& faceMap, std::vector<GluetagToPlane>& gtMap);

    void planar(QVector3D const &A, QVector3D const &B, QVector3D const &C, QVector2D& a, QVector2D& b, QVector2D& c);
    void planar(QVector3D const &P1, QVector3D const &P2, QVector3D const &Pu, QVector2D const &p1, QVector2D const &p2, QVector2D const &p3prev,  QVector2D& pu);
};

#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

#include <QOpenGLFunctions_4_5_Core>
#include <QVector3D>

#include <random>

#include "edge.h"
#include "gluetag.h"
#include "gluetagtoplane.h"
#include "facetoplane.h"

#include "utility.h"

#define TEMP_MAX 7000.0
#define EPOCH 3.0
#define TEMP_MIN 1.0
#define RESET 50.0
#define BOLTZMANC 0.000013806403

class Gluetag;
class GluetagToPlane;
class FaceToPlane;

class Graph
{
public:
    Graph();
    ~Graph();

    void initC();
    void nextC();

    void oglPlanar(std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colorsLines, QMatrix4x4& center);
    void oglLines(std::vector<QVector3D>& lineVertices, std::vector<QVector3D>& lineColors);
    void oglGluetags(std::vector<QVector3D>& gtVertices, std::vector<GLushort>& gtIndices, std::vector<QVector3D>& gtColors);

    void addFace(Facet facet);

    double energy();
    bool over();
private:
    /** Calculated once, always valid **/
    std::map<int, Facet> _facets;
    std::vector<Edge> _edges;
    std::vector<Gluetag> _gluetags;

    /** Calculated each epoch **/
    std::vector<Edge> _mspEdges;
    std::vector<Edge> _cutEdges;
    std::vector<Gluetag> _necessaryGluetags;
    std::vector<std::vector<int>> _tree;

    /** Best calculated Solution **/
    std::vector<Edge> _C;
    std::vector<Gluetag> _Cgt;
    double _Cenergy;

    double temperature;
    int resetCounter;

    void calculateDual();
    void calculateMSP();
    void calculateGlueTags();

    int unfold(std::vector<std::vector<int>> const &edges, ulong index, std::vector<bool>& discovered, ulong parent, std::vector<FaceToPlane>& faceMap, std::vector<GluetagToPlane>& gtMap);
    void move();

    void changeFaces();
    void changeGluetags();

    void resetTree();
    void planar(QVector3D const &A, QVector3D const &B, QVector3D const &C, QVector2D& a, QVector2D& b, QVector2D& c);
    void planar(QVector3D const &P1, QVector3D const &P2, QVector3D const &Pu, QVector2D const &p1, QVector2D const &p2, QVector2D const &p3prev,  QVector2D& pu);
    int getFacetID(Facet facet);
    QVector3D faceCenter(Facet facet);
    bool hasEdge(Edge& edge);
    bool isSingleComponent(std::vector<std::vector<int>>& adjacenceList);
    bool isAcyclic(std::vector<std::vector<int>> const &graph, ulong start, std::vector<bool> &discovered, int parent);
};

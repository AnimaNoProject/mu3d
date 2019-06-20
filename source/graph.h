#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

#include <QOpenGLFunctions_4_5_Core>
#include <QVector3D>

#include <random>

#include <thread>
#include <future>

#include "edge.h"
#include "gluetag.h"
#include "gluetagtoplane.h"
#include "facetoplane.h"

#include "utility.h"

#define TEMP_MAX 400000.0
#define EPOCH 1.0
#define TEMP_MIN 10.0

class Gluetag;
class GluetagToPlane;
class FaceToPlane;

class Graph
{
public:
    Graph();
    ~Graph();

    void initializeState();
    bool neighbourState();

    void oglPlanar(std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colorsLines, QMatrix4x4& center);
    void oglLines(std::vector<QVector3D>& lineVertices, std::vector<QVector3D>& lineColors);
    void oglGluetags(std::vector<QVector3D>& gtVertices, std::vector<GLushort>& gtIndices, std::vector<QVector3D>& gtColors);

    void addFace(Facet facet);

    double energy();
    bool over();

    double temperature;
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
    std::vector<FaceToPlane> _CplanarFaces;
    std::vector<GluetagToPlane> _CplanarGluetags;
    double _Cenergy;

    std::vector<FaceToPlane> _planarFaces;
    std::vector<GluetagToPlane> _planarGluetags;

    void calculateDual();
    void calculateMSP();
    void calculateGlueTags();

    std::pair<double, double> unfold();
    std::pair<double, double> unfold(ulong index, std::vector<bool>& discovered, ulong parent);

    double gtOverlapArea(size_t index);
    double fOverlapArea(ulong index, std::vector<bool>& discovered, ulong parent);

    double gtgtOverlap(GluetagToPlane& gt);
    double gtfOverlap(GluetagToPlane& gt, std::vector<bool>& discovered);

    void randomMove();

    void resetTree();
    void planar(QVector3D const &A, QVector3D const &B, QVector3D const &C, QVector2D& a, QVector2D& b, QVector2D& c);
    void planar(QVector3D const &P1, QVector3D const &P2, QVector3D const &Pu, QVector2D const &p1, QVector2D const &p2, QVector2D const &p3prev,  QVector2D& pu);

    QVector3D faceCenter(Facet facet);

    bool isSingleComponent(std::vector<std::vector<int>> &adjacenceList);
    bool isAcyclic(std::vector<std::vector<int>> const &graph, ulong start, std::vector<bool>& discovered, int parent);

    int find(Facet facet);
    bool find(Edge& edge);
};

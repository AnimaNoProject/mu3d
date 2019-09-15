#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

#include <QOpenGLFunctions_4_5_Core>
#include <QVector3D>

#include <random>

#include <algorithm>
#include <iostream>
#include <string>

#include <thread>
#include <future>

#include "edge.h"
#include "gluetag.h"
#include "gluetagtoplane.h"
#include "facetoplane.h"

#include "utility.h"

#define TEMP_MAX 100000.0
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

    int initBruteForce();
    bool nextBruteForce();

    void oglPlanar(std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colorsLines, QMatrix4x4& center);
    void oglLines(std::vector<QVector3D>& lineVertices, std::vector<QVector3D>& lineColors);
    void oglGluetags(std::vector<QVector3D>& gtVertices, std::vector<GLushort>& gtIndices, std::vector<QVector3D>& gtColors);

    void postProcessPlanar(std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colorsLines, QMatrix4x4& center);

    void addFace(Facet facet);

    double energy();
    bool over();
    bool finishedBruteFroce();

    double temperature;
private:
    int n;
    int r;
    std::vector<bool> v;

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

    bool calculateMSP(std::vector<int> edges);
    void calculateMSP();

    void calculateGlueTags(std::vector<Gluetag> gluetags);

    void unfoldTriangles();
    void unfoldTriangles(int index, std::vector<bool>& discovered, int parent);
    void unfoldGluetags();

    double findTriangleOverlaps();
    double findGluetagOverlaps();

    void randomMove();

    void resetTree();

    QVector3D faceCenter(Facet facet);

    bool isSingleComponent(std::vector<std::vector<int>> &adjacenceList);
    bool isAcyclic(std::vector<std::vector<int>> const &graph, int start, std::vector<bool>& discovered, int parent);

    int find(Facet facet);
    bool find(Edge& edge);

    void initEdgeWeight();

    int factorial(int n);
};

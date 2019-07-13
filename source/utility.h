#pragma once
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include <QMatrix4x4>
#include <QVector3D>

#include <fstream>
#include <iostream>
#include <list>

#include <math.h>

#include "graph.h"

struct shEdge
{
public:

    shEdge(QVector2D& from, QVector2D& to)
    {
        _from = from;
        _to = to;
    }

    QVector2D _from;
    QVector2D _to;
};

class Utility
{
public:
    static void createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], QOpenGLBuffer& ibo, std::vector<QVector3D>& vertices, std::vector<GLushort>& indices, std::vector<QVector3D>& colors);
    static void createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors);
    static QVector3D pointToVector(CGAL::Point_3<CGAL::Simple_cartesian<double>>& point);

    static bool intersects(QVector2D& p1, QVector2D& q1, QVector2D& p2, QVector2D& q2);
    static double intersectionArea(QVector2D& p1, QVector2D& q1, QVector2D& r1, QVector2D& p2, QVector2D& q2, QVector2D& r2);

    static void planar(QVector3D const &A, QVector3D const &B, QVector3D const &C, QVector2D& a, QVector2D& b, QVector2D& c);
    static void planar(QVector3D const &P1, QVector3D const &P2, QVector3D const &Pu, QVector2D const &p1, QVector2D const &p2, QVector2D const &p3prev,  QVector2D& pu);
private:
    static bool intersectionPoint(QVector2D& p1, QVector2D& q1, QVector2D& p2, QVector2D& q2, QVector2D& ip);
    static bool pointInTriangle(QVector2D& p, QVector2D& v1, QVector2D& v2, QVector2D& v3);

    static int orientation(QVector2D& p, QVector2D& q, QVector2D& r);
    static bool onSegment(QVector2D& p, QVector2D& q, QVector2D& r);
    static float sign(QVector2D& p1, QVector2D& p2, QVector2D& p3);

    static bool compareVector2D(const QVector2D& p1, const QVector2D& p2);
    static float getcounterclockwise(const QVector2D& p);
};

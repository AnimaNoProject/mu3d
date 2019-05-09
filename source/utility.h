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

class Utility
{
public:
    static void createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], QOpenGLBuffer& ibo, std::vector<QVector3D> vertices, std::vector<GLushort> indices, std::vector<QVector3D> colors);
    static void createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], std::vector<QVector3D> vertices, std::vector<QVector3D> colors);
    static QVector3D pointToVector(CGAL::Point_3<CGAL::Simple_cartesian<double>> point);

    static bool intersects(QVector2D p1, QVector2D q1, QVector2D p2, QVector2D q2);
private:
    static int orientation(QVector2D p, QVector2D q, QVector2D r);
    static bool onSegment(QVector2D p, QVector2D q, QVector2D r);
};

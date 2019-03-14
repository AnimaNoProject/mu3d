#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <fstream>
#include <iostream>
#include <QVector3D>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;

class Model
{
public:
    Model(const char* filename);
    Model();
    ~Model();
    std::vector<QVector3D> getVertices();
    std::vector<GLushort> getIndices();
    QMatrix4x4 getModelMatrix();
private:
    Polyhedron _mesh;
    QMatrix4x4 _modelMatrix;
    std::vector<QVector3D> _vertices;
    std::vector<GLushort> _indices;

    void debugModel();
};

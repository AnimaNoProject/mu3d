#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>

#include <boost/graph/kruskal_min_spanning_tree.hpp>

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include <QMatrix4x4>
#include <QVector3D>

#include <fstream>
#include <iostream>
#include <list>

#include "oglwidget.h"

class OGLWidget;

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Kernel::Vector_3 Vector;
typedef Kernel::Point_3 Point;

typedef boost::graph_traits<Polyhedron>::vertex_descriptor VertexDescriptor;
typedef boost::graph_traits<Polyhedron>::vertex_iterator VertexIterator;
typedef boost::graph_traits<Polyhedron>::edge_descriptor EdgeDescriptor;

typedef std::map<VertexDescriptor, int> VertexIndexMap;
typedef boost::associative_property_map<VertexIndexMap> VertexIndexPmap;


class Model
{
public:
    Model(const char* filename, QOpenGLShaderProgram* program);
    Model(QOpenGLShaderProgram* program);
    ~Model();
    void draw();
    void switchRenderMode();

    void kruskal();
private:
    Polyhedron _mesh;
    QMatrix4x4 _modelMatrix;
    std::vector<QVector3D> _vertices;
    std::vector<GLushort> _indices;

    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo;
    QOpenGLBuffer _ibo;

    QOpenGLShaderProgram* _program;
    OGLWidget* _context;

    void createGLModelContext();
    void debugModel();

    const QVector4D _lineColor = QVector4D(0.0, 0.0, 0.0, 1.0);
    const QVector4D _fillColor = QVector4D(1.0, 1.0, 1.0, 1.0);

    bool _wireframe = false;
};

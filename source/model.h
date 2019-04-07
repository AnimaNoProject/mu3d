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

#include "oglwidget.h"
#include "graph.h"

class OGLWidget;

class Model
{
public:
    Model(const char* filename, QOpenGLShaderProgram* program);
    Model(QOpenGLShaderProgram* program);
    ~Model();
    void draw();
    void switchRenderMode();
    void showGluetags();
private:
    Graph _graph;
    Polyhedron _mesh;
    QMatrix4x4 _modelMatrix;
    std::vector<QVector3D> _vertices;
    std::vector<QVector3D> _colors;
    std::vector<GLushort> _indices;

    std::vector<QVector3D> _lineVertices;
    std::vector<QVector3D> _lineColors;

    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo[2];
    QOpenGLBuffer _ibo;

    QOpenGLVertexArrayObject _vaoLines;
    QOpenGLBuffer _vboLines[2];

    QOpenGLShaderProgram* _program;
    OGLWidget* _context;

    GLushort _modelPolygons;
    bool _wireframe = false;
    bool _showgluetags = true;

    void createGLModelContext();
};

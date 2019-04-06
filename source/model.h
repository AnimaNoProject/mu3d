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
private:
    Graph _graph;
    Polyhedron _mesh;
    QMatrix4x4 _modelMatrix;
    std::vector<QVector3D> _vertices;
    std::vector<GLushort> _indices;
    std::vector<QVector3D> _mspVertices;
    std::vector<QVector3D> _cutVertices;
    std::vector<QVector3D> _stickVertices;

    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo;
    QOpenGLBuffer _ibo;

    QOpenGLVertexArrayObject _vaoDual;
    QOpenGLBuffer _vboMSP;

    QOpenGLVertexArrayObject _vaoCut;
    QOpenGLBuffer _vboCut;


    QOpenGLVertexArrayObject _vaoStick;
    QOpenGLBuffer _vboStick;

    QOpenGLShaderProgram* _program;
    OGLWidget* _context;

    const QVector4D _lineColor = QVector4D(0.0, 0.0, 0.0, 1.0);
    const QVector4D _fillColor = QVector4D(1.0, 1.0, 1.0, 1.0);
    const QVector4D _mspColor = QVector4D(0.0, 1.0, 0.0, 1.0);
    const QVector4D _cutColor = QVector4D(1.0, 0.0, 0.0, 1.0);

    bool _wireframe = false;

    void createGLModelContext();
};

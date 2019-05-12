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
#include "utility.h"

class OGLWidget;

class Model
{
public:
    Model(const char* filename);
    Model();
    ~Model();
    void draw(QOpenGLShaderProgram* program);
    void drawPlanarPatch(QOpenGLShaderProgram* program);
    void switchRenderMode();
    void showGluetags();
    void unfold();

    bool finishedAnnealing();

    void createGLModelContext(QOpenGLShaderProgram* program);
    void recalculate(QOpenGLShaderProgram* program);

    Graph _graph;
    QMatrix4x4 _modelMatrixPlanar;

private:
    Polyhedron _mesh;
    QMatrix4x4 _modelMatrix;

    std::vector<QVector3D> _vertices;
    std::vector<QVector3D> _colors;
    std::vector<GLushort> _indices;

    std::vector<QVector3D> _verticesGT;
    std::vector<QVector3D> _colorsGT;
    std::vector<GLushort> _indicesGT;

    std::vector<QVector3D> _lineVertices;
    std::vector<QVector3D> _lineColors;

    std::vector<QVector3D> _planarVertices;
    std::vector<QVector3D> _planarColors;

    std::vector<QVector3D> _planarLines;
    std::vector<QVector3D> _planarLinesColors;

    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo[2];
    QOpenGLBuffer _ibo;

    QOpenGLVertexArrayObject _vaoGT;
    QOpenGLBuffer _vboGT[2];
    QOpenGLBuffer _iboGT;

    QOpenGLVertexArrayObject _vaoLines;
    QOpenGLBuffer _vboLines[2];

    QOpenGLVertexArrayObject _vaoPlanar;
    QOpenGLBuffer _vboPlanar[2];

    QOpenGLVertexArrayObject _vaoPlanarLines;
    QOpenGLBuffer _vboPlanarLines[2];

    OGLWidget* _context;

    GLushort _modelPolygons;
    bool _wireframe = false;
    bool _showgluetags = true;

    void clearOGL();
    void drawPolygons(QOpenGLVertexArrayObject& vao, unsigned long triangles, float offset);
    void drawLines(QOpenGLVertexArrayObject& vao);
};

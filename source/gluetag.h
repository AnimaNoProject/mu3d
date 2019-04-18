#pragma once
#include "edge.h"

#include <QOpenGLFunctions_4_5_Core>

class Gluetag
{
public:
    Gluetag(Edge& edge);
    void getVertices(std::vector<QVector3D>& vertices, std::vector<GLushort>& indices, std::vector<QVector3D>& colors);

    int _placedFace;
    int _targetFace;
    Edge _edge;
private:
    QVector3D _bl;
    QVector3D _br;
    QVector3D _tl;
    QVector3D _tr;
    QVector3D _color;

    QVector3D hex2rgb(std::string hex);

    static size_t id;
};

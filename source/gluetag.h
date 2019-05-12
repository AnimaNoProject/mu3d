#pragma once
#include "edge.h"
#include "utility.h"
#include "QVector2D"

#include <QOpenGLFunctions_4_5_Core>

class Gluetag
{
public:
    Gluetag(Edge& edge, bool flag);
    int _placedFace;
    int _targetFace;
    Edge _edge;
    QVector3D _bl;
    QVector3D _br;
    QVector3D _tl;
    QVector3D _tr;
    QVector3D _color;

    int _cost;

    void getVertices(std::vector<QVector3D>& vertices, std::vector<GLushort>& indices, std::vector<QVector3D>& colors);
    bool operator<(const Gluetag& other) const;
private:
    QVector3D hex2rgb(std::string hex);

    static size_t id;
};

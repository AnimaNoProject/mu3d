#pragma once
#include "edge.h"

#include <QOpenGLFunctions_4_5_Core>

class Gluetag
{
public:
    Gluetag(Edge& edge);
    void addVertices(std::vector<QVector3D>& vertices, std::vector<GLushort>& indices, std::vector<QVector3D>& colors);
private:
    Edge _edge;
    int _placedFace;
};

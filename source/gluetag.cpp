#include "gluetag.h"

Gluetag::Gluetag(Edge& edge)
{
    _edge = edge;
    _placedFace = edge._sFace;
}

void Gluetag::addVertices(std::vector<QVector3D>& vertices, std::vector<GLushort>& indices, std::vector<QVector3D>& colors)
{
    int index = int(vertices.size())-1;

    QVector3D baseBL = QVector3D(float(_edge._halfedge->vertex()->point().x()),
                                 float(_edge._halfedge->vertex()->point().y()),
                                 float(_edge._halfedge->vertex()->point().z()));

    QVector3D baseBR = QVector3D(float(_edge._halfedge->prev()->vertex()->point().x()),
                                float(_edge._halfedge->prev()->vertex()->point().y()),
                                float(_edge._halfedge->prev()->vertex()->point().z()));

    QVector3D up = baseBR.normalized() / 4;
    QVector3D side = (baseBL - baseBR).normalized() / 4;

    QVector3D baseTL = baseBL + up - side;
    QVector3D baseTR = baseBR + up + side;

    vertices.push_back(baseBL); // bottom left = -3
    vertices.push_back(baseBR); // bottom right = -2
    vertices.push_back(baseTR); // top right = -1
    vertices.push_back(baseTL); // top left = 0

    index += 4;

    indices.push_back(GLushort(index-3));
    indices.push_back(GLushort(index-2));
    indices.push_back(GLushort(index-1));
    indices.push_back(GLushort(index-3));
    indices.push_back(GLushort(index-1));
    indices.push_back(GLushort(index));

    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
}

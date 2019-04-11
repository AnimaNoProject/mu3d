#include "gluetag.h"

Gluetag::Gluetag(Edge& edge)
{
    _edge = edge;
    _placedFace = edge._sFace;
    _targetFace = edge._tFace;


    // get the bottom left corner of the base
    _bl = QVector3D(float(_edge._halfedge->vertex()->point().x()),
                                 float(_edge._halfedge->vertex()->point().y()),
                                 float(_edge._halfedge->vertex()->point().z()));

    // get the bottom right corner of the base
    _br = QVector3D(float(_edge._halfedge->prev()->vertex()->point().x()),
                                float(_edge._halfedge->prev()->vertex()->point().y()),
                                float(_edge._halfedge->prev()->vertex()->point().z()));

    // get the face which the glue tag points to
    Facet face = _edge[_targetFace];

    // get the vertex where the gluetag is extruded to
    QVector3D target;
    Polyhedron::Halfedge_around_facet_circulator hfc = face->facet_begin();
    do
    {
        target = QVector3D(float(hfc->vertex()->point().x()),
                           float(hfc->vertex()->point().y()),
                           float(hfc->vertex()->point().z()));

        // if the vertex is neither the bottom right or bottom left it is the target
        if(target != _bl && target != _br)
        {
            break;
        }
    } while (++hfc != face->facet_begin());


    // top edge of the gluetag is 1/3 of the size of the base edge
    QVector3D side = (_br - _bl) / 4;

    // adjust size of base of gluetag
    _bl = _bl + side;
    _br = _br - side;

    // top right and top left calculation, top edge of the gluetag is at 1/4 of the height of the targeted face
    _tr = _br + (target - _br) / 4 - side;
    _tl = _bl + (target - _bl) / 4 + side;
}

void Gluetag::getVertices(std::vector<QVector3D>& vertices, std::vector<GLushort>& indices, std::vector<QVector3D>& colors)
{
    // get the last index
    int index = int(vertices.size())-1;

    vertices.push_back(_bl); // bottom left = -3
    vertices.push_back(_br); // bottom right = -2
    vertices.push_back(_tr); // top right = -1
    vertices.push_back(_tl); // top left = 0

    // index + 4 because, 4 vertices added
    index += 4;

    // indices for the two triangles
    indices.push_back(GLushort(index-3));
    indices.push_back(GLushort(index-2));
    indices.push_back(GLushort(index));
    indices.push_back(GLushort(index-3));
    indices.push_back(GLushort(index-1));
    indices.push_back(GLushort(index));

    // colors, temporary blue
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
    colors.push_back(QVector3D(0.2f, 0.2f, 0.8f));
}

#include "gluetag.h"

//QVector3D Gluetag::_color = QVector3D(0.0f,0.5f,1.0f);

size_t Gluetag::id = 0;

Gluetag::Gluetag(Edge& edge, bool flag) : _edge(edge), _probability(0)
{
    Facet face;

    if(flag)
    {
        _placedFace = edge._sFace;
        _targetFace = edge._tFace;
        face = edge._tFacetHandle;
    }
    else
    {
        _placedFace = edge._tFace;
        _targetFace = edge._sFace;
        face = edge._sFacetHandle;
    }

    // get the bottom left corner of the base
    _bl = QVector3D(float(_edge._halfedge->vertex()->point().x()),
                                 float(_edge._halfedge->vertex()->point().y()),
                                 float(_edge._halfedge->vertex()->point().z()));

    // get the bottom right corner of the base
    _br = QVector3D(float(_edge._halfedge->prev()->vertex()->point().x()),
                                float(_edge._halfedge->prev()->vertex()->point().y()),
                                float(_edge._halfedge->prev()->vertex()->point().z()));

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


    // top edge of the gluetag is 1/4 of the size of the base edge
    QVector3D side = (_br - _bl) / 8;

    _tr = _br + (target - _br) / 5 - side;
    _tl = _bl + (target - _bl) / 5 + side;

    _color = hex2rgb(colors[id++ % colors.size()]);
}

void Gluetag::getVertices(std::vector<QVector3D>& vertices, std::vector<GLushort>& indices, std::vector<QVector3D>& colors)
{
    // get the last index
    int index = int(vertices.size())-1;

    vertices.push_back(_bl); // bottom left = -3
    vertices.push_back(_br); // bottom right = -2
    vertices.push_back(_tr); // top right = -1
    vertices.push_back(_tl); // top left = 0

    // index + 4 because, 4 vertices added2
    index += 4;

    // indices for the two triangles
    indices.push_back(GLushort(index-3));
    indices.push_back(GLushort(index-2));
    indices.push_back(GLushort(index));
    indices.push_back(GLushort(index));
    indices.push_back(GLushort(index-2));
    indices.push_back(GLushort(index-1));

    colors.push_back(_color);
    colors.push_back(_color);
    colors.push_back(_color);
    colors.push_back(_color);
}

bool Gluetag::operator<(const Gluetag& other) const
{
    return _probability < other._probability;
}

QVector3D Gluetag::hex2rgb(std::string hex)
{
    if(hex[0] == '#')
        hex = hex.erase(0, 1);

    int rgb[3];

    std::stringstream ss;
    std::string str;

    for(ulong i = 0; i < 3; i++)
    {
        str = hex.substr(i * 2, 2);
        ss << std::hex << str;
        ss >> rgb[i];
        ss.clear();
    }

    return QVector3D(float(rgb[0]) / 255.0f, float(rgb[1]) / 255.0f, float(rgb[2]) / 255.0f);
}

#include "model.h"

// default cube size
const float width = 4;
const float height = 4;
const float depth = 4;

// default cube vertices
const std::vector<QVector3D> vertices = {
    // front
    QVector3D(-width / 2.0f, -height / 2.0f,  depth / 2.0f),
    QVector3D(width / 2.0f, -height / 2.0f,  depth / 2.0f),
    QVector3D(width / 2.0f,  height / 2.0f,  depth / 2.0f),
    QVector3D(-width / 2.0f,  height / 2.0f,  depth / 2.0f),
    // back
    QVector3D(-width / 2.0f, -height / 2.0f, -depth / 2.0f),
    QVector3D(width / 2.0f, -height / 2.0f, -depth / 2.0f),
    QVector3D(width / 2.0f,  height / 2.0f, -depth / 2.0f),
    QVector3D(-width / 2.0f,  height / 2.0f, -depth / 2.0f)
};

// default cube vertices
const std::vector<QVector3D> vertices2 = {
    // front
    QVector3D(-width, -height,  depth),
    QVector3D(width, -height,  depth),
    QVector3D(width,  height,  depth),
    QVector3D(-width,  height,  depth),
    // back
    QVector3D(-width, -height, -depth),
    QVector3D(width, -height, -depth),
    QVector3D(width,  height, -depth),
    QVector3D(-width,  height, -depth)
};

// default cube indices
const std::vector<GLushort> indices = {// front
                                0, 1, 2,
                                2, 3, 0,
                                // top
                                1, 5, 6,
                                6, 2, 1,
                                // back
                                7, 6, 5,
                                5, 4, 7,
                                // bottom
                                4, 0, 3,
                                3, 7, 4,
                                // left
                                4, 5, 1,
                                1, 0, 4,
                                // right
                                3, 2, 6,
                                6, 7, 3};

Model::Model(const char* filename)
{
    // open file buffer
    std::filebuf filebuffer;
    if(filebuffer.open(filename, std::ios::in))
    {
        // read .off file into CGAL::Polyhedron_3
        std::istream is(&filebuffer);
        CGAL::read_off(is, _mesh);
        filebuffer.close();
    }

    _vertices = vertices2;
    _indices = indices;

    debugModel(); // print debugging relevant informations
}

std::vector<QVector3D> Model::getVertices()
{
    return _vertices;
}

std::vector<GLushort> Model::getIndices()
{
    return _indices;
}

QMatrix4x4 Model::getModelMatrix()
{
    return _modelMatrix;
}

void Model::debugModel()
{
    CGAL::set_ascii_mode( std::cout);
    for ( Polyhedron::Vertex_iterator v = _mesh.vertices_begin(); v != _mesh.vertices_end(); ++v)
        std::cout << v->point() << std::endl;
}

Model::Model()
{
    // if no .off file is specified use standard cube to draw
    _vertices = vertices;
    _indices = indices;
}

Model::~Model()
{
}

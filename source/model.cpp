#include "model.h"

// default cube size
const float width = 4;
const float height = 4;
const float depth = 4;

// default cube vertices
const std::vector<QVector3D> defaultCubeVertices = {
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

// default cube indices
const std::vector<GLushort> defaultCubeIndices = {// front
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

Model::Model(const char* filename, QOpenGLShaderProgram* program)
{
    _program = program;

    // open file buffer
    std::filebuf filebuffer;
    if(filebuffer.open(filename, std::ios::in))
    {
        // read .off file into CGAL::Polyhedron_3
        std::istream is(&filebuffer);
        CGAL::read_off(is, _mesh);
        filebuffer.close();
    }

    // get all vertices
    for ( Polyhedron::Vertex_iterator v = _mesh.vertices_begin(); v != _mesh.vertices_end(); ++v)
    {
        _vertices.push_back(QVector3D(v->point().x(), v->point().y(), v->point().z()));
    }

    // loop through all facets
    for (Polyhedron::Facet_iterator fi = _mesh.facets_begin(); fi != _mesh.facets_end(); ++fi)
    {
        // get the first facet
        Polyhedron::Halfedge_around_facet_circulator hfc = fi->facet_begin();
        // assert that all facets are triangles
        CGAL_assertion(CGAL::circulator_size(hfc) >= 3);
        do
        {
            uint foundindex = 0;
            // loop through vertices and save their index if they match
            for(std::vector<int>::size_type i = 0; i != _vertices.size(); ++i)
            {
                if((float)hfc->vertex()->point().x() == _vertices[i].x() &&
                    (float)hfc->vertex()->point().y() == _vertices[i].y() &&
                    (float)hfc->vertex()->point().z() == _vertices[i].z())
                {
                    foundindex = i;
                    break;
                }
            }
            _indices.push_back(foundindex);
        } while (++hfc != fi->facet_begin());
    }

    _modelMatrix.setToIdentity();

    createGLModelContext();

    //debugModel();
}

void Model::createGLModelContext()
{
    // delcare Vertex and Index buffer
    _vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    _ibo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    _program->bind();

    // create and bind VAO
    _vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);

    // get openglfunctions from the current context (important OGLWidget needs to call makeCurrent)
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    // create VBO and allocate buffer
    _vbo.create();
    _vbo.bind();
    _vbo.allocate(_vertices.data(), _vertices.size() * sizeof(QVector3D));
    _vbo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    _vbo.release();

    // create IBO and allocate buffer
    _ibo.create();
    _ibo.bind();
    _ibo.allocate(_indices.data(), _indices.size() * sizeof(GLushort));
    _ibo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    vaoBinder.release();
    _ibo.release();

    _program->release();
}

void Model::draw()
{
    // get opengl functions
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();
    // bind the VAO
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    // set the model Matrix
    _program->setUniformValue(_program->uniformLocation("modelMatrix"), _modelMatrix);
    // draw all triangles
    f->glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    f->glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_SHORT, 0);
}

void Model::debugModel()
{
    for ( Polyhedron::Vertex_iterator v = _mesh.vertices_begin(); v != _mesh.vertices_end(); ++v)
    {
        std::cout << "Vertex: " << v->point().x() << "," << v->point().y() << "," << v->point().z() << std::endl;
    }

    for (Polyhedron::Facet_iterator fi = _mesh.facets_begin(); fi != _mesh.facets_end(); ++fi)
    {
        Polyhedron::Halfedge_around_facet_circulator hfc = fi->facet_begin();
        CGAL_assertion(CGAL::circulator_size(hfc) >= 3);
        std::cout << "Triangle: ";
        do
        {
            std::cout << "V(" << hfc->vertex()->point().x() << "," << hfc->vertex()->point().y() << ","  << hfc->vertex()->point().z() << ")";
        } while (++hfc != fi->facet_begin());
        std::cout << std::endl;
    }
}

Model::Model(QOpenGLShaderProgram* program)
{
    _program = program;

    // if no .off file is specified use standard cube to draw
    _vertices = defaultCubeVertices;
    _indices = defaultCubeIndices;
    _modelMatrix.setToIdentity();

    createGLModelContext();
}

Model::~Model()
{
}

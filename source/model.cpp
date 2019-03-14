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

    _vertices = vertices2;
    _indices = indices;
    _modelMatrix.setToIdentity();

    createGLModelContext();

    debugModel(); // print debugging relevant informations
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
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();

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
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // bind the VAO
    QOpenGLVertexArrayObject::Binder vaoBinder(&_vao);
    // set the model Matrix
    _program->setUniformValue(_program->uniformLocation("modelMatrix"), _modelMatrix);
    // draw all triangles
    f->glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_SHORT, 0);
}

void Model::debugModel()
{
    CGAL::set_ascii_mode( std::cout);
    for ( Polyhedron::Vertex_iterator v = _mesh.vertices_begin(); v != _mesh.vertices_end(); ++v)
        std::cout << v->point() << std::endl;
}

Model::Model(QOpenGLShaderProgram* program)
{
    _program = program;

    // if no .off file is specified use standard cube to draw
    _vertices = vertices;
    _indices = indices;
    _modelMatrix.setToIdentity();

    createGLModelContext();
}

Model::~Model()
{
}

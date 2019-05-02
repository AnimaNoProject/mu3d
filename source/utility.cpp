#include "utility.h"

QVector3D Utility::pointToVector(CGAL::Point_3<CGAL::Simple_cartesian<double>> point)
{
    return QVector3D(point.x(), point.y(), point.z());
}

void Utility::createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], QOpenGLBuffer& ibo, std::vector<QVector3D> vertices, std::vector<GLushort> indices, std::vector<QVector3D> colors)
{
    // delcare Vertex and Index buffer
    vbo[0] = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[1] = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ibo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    // create and bind VAO
    vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

    // get openglfunctions from the current context (important OGLWidget needs to call makeCurrent)
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    // create vbo for vertices
    vbo[0].create();
    vbo[0].bind();
    vbo[0].allocate(vertices.data(), int(vertices.size() * sizeof(QVector3D)));
    vbo[0].setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbo[0].release();

    // create vbo for colors
    vbo[1].create();
    vbo[1].bind();
    vbo[1].allocate(colors.data(), int(colors.size() * sizeof(QVector3D)));
    vbo[1].setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbo[1].release();

    // create IBO and allocate buffer
    ibo.create();
    ibo.bind();
    ibo.allocate(indices.data(), int(indices.size() * sizeof(GLushort)));
    ibo.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    vaoBinder.release();
    ibo.release();

    vbo[0].destroy();
    vbo[1].destroy();
    ibo.destroy();
}

void Utility::createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], std::vector<QVector3D> vertices, std::vector<QVector3D> colors)
{
    // delcare Vertex
    vbo[0] = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vbo[1] = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    // get openglfunctions from the current context (important OGLWidget needs to call makeCurrent)
    QOpenGLFunctions_4_5_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_5_Core>();

    vao.create();
    QOpenGLVertexArrayObject::Binder vaoLinesBinder(&vao);

    // create vbo for vertices
    vbo[0].create();
    vbo[0].bind();
    vbo[0].allocate(vertices.data(), int(vertices.size() * sizeof(QVector3D)));
    vbo[0].setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbo[0].release();

    // create vbo for colors
    vbo[1].create();
    vbo[1].bind();
    vbo[1].allocate(colors.data(), int(colors.size() * sizeof(QVector3D)));
    vbo[1].setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    vbo[1].release();
    vaoLinesBinder.release();
}

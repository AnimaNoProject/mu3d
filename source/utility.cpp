#include "utility.h"

QVector3D Utility::pointToVector(CGAL::Point_3<CGAL::Simple_cartesian<double>> point)
{
    return QVector3D(float(point.x()), float(point.y()), float(point.z()));
}

QVector2D Utility::intersectionPoint(QVector2D p1, QVector2D p2, QVector2D p3, QVector2D p4)
{
    float xnum = (p1.x() * p2.y() - p1.y() * p2.x()) * (p3.x() - p4.x()) -
                  (p1.x() - p2.x()) * (p3.x() * p4.y() - p3.y() * p4.x());
    float xden = (p1.x() - p2.x()) * (p3.y() - p4.y()) - (p1.y() - p2.y()) * (p3.x() - p4.x());

    float ynum = (p1.x() * p2.y() - p1.y() * p2.y()) * (p3.y() - p4.y()) -
              (p1.y() - p2.y()) * (p3.x() * p4.y() - p3.y() * p4.x());
    float yden = (p1.x() - p2.x()) * (p3.y() - p4.y()) - (p1.y() - p2.y()) * (p3.x() - p4.x());

    return QVector2D(xnum/xden, ynum/yden);
}

bool Utility::pointInTriangle(QVector2D p, QVector2D v1, QVector2D v2, QVector2D v3)
{
    float d1, d2, d3;
    bool hasNeg;
    bool hasPos;

    d1 = sign(p, v1, v2);
    d2 = sign(p, v2, v3);
    d3 = sign(p, v3, v1);

    hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNeg && hasPos);
}

float Utility::sign(QVector2D p1, QVector2D p2, QVector2D p3)
{
    return (p1.x() - p3.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x()) * (p1.y() - p3.y());
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
    vbo[0].destroy();
    vbo[1].destroy();
    vao.destroy();

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

bool Utility::intersects(QVector2D p1, QVector2D q1, QVector2D p2, QVector2D q2)
{
    if((p1 == p2 && q1 != q2) || (p1 == q2 && q1 != p2) || (q1 == p2 && p1 != q2) || (q1 == q2 && p1 != p2))
    {
        return false;
    }

    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);

    if(o1 != o2 && o3 != o4)
    {
#ifndef NDEBUG
        std::cout << "p1 = [" << p1.x() << "," << p1.y() << "]"
                  << "q1 = [" << q1.x() << "," << q1.y() << "]" << std::endl
                  << "p2 = [" << p2.x() << "," << p2.y() << "]"
                  << "q2 = [" << q2.x() << "," << q2.y() << "]" << std::endl << std::endl;
#endif
        return true;
    }

    if    ((o1 == 0 && onSegment(p1, p2, q1))
        || (o2 == 0 && onSegment(p1, q2, q1))
        || (o3 == 0 && onSegment(p2, p1, q2))
        || (o4 == 0 && onSegment(p2, q1, q2)))
    {
#ifndef NDEBUG
        std::cout << "onSegment && o1/4 = 0" << std::endl;
#endif
        return true;
    }

    return false;
}

int Utility::orientation(QVector2D p, QVector2D q, QVector2D r)
{
    float val = (q.y() - p.y()) * (r.x() - q.x()) -
                (q.x() - p.x()) * (r.y() - q.y());

    if(val == 0)
    {
        return 0;
    }
    else if (val > 0)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

bool Utility::onSegment(QVector2D p, QVector2D q, QVector2D r)
{
    if(q.x() <= std::max(p.x(), r.x()) && q.x() >= std::min(p.x(), r.x()) &&
       q.y() <= std::max(p.y(), r.y()) && q.y() >= std::min(p.y(), r.y()))
    {
        return true;
    }

    return false;
}

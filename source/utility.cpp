#include "utility.h"

void Utility::planar(QVector3D const &A, QVector3D const &B, QVector3D const &C, QVector2D& a, QVector2D& b, QVector2D& c)
{
    float lengthAB = (A - B).length();

    a = QVector2D(0, 0);
    b = QVector2D(lengthAB, 0);

    float s = QVector3D::crossProduct(B - A, C - A).length() / float(std::pow(lengthAB, 2));
    float cl = QVector3D::dotProduct(B - A, C - A) / float(std::pow(lengthAB, 2));

    c = QVector2D(a.x() + cl * (b.x() - a.x()) - s * (b.y() - a.y()),
                  a.y() + cl * (b.y() - a.y()) + s * (b.x() - a.x()));
}

void Utility::planar(QVector3D const &P1, QVector3D const &P2, QVector3D const &Pu, QVector2D const &p1, QVector2D const &p2, QVector2D const &p3prev,  QVector2D& pu)
{
    float length = (p1 - p2).length();

    float s = QVector3D::crossProduct((P2 - P1), (Pu - P1)).length() / float(std::pow(length, 2));
    float unkown = QVector3D::dotProduct((P2 - P1), (Pu - P1)) / float(std::pow(length, 2));

    QVector2D pu1 = QVector2D(p1.x() + unkown * (p2.x() - p1.x()) + s * (p2.y() - p1.y()),
                              p1.y() + unkown * (p2.y() - p1.y()) - s * (p2.x() - p1.x()));

    QVector2D pu2 = QVector2D(p1.x() + unkown * (p2.x() - p1.x()) - s * (p2.y() - p1.y()),
                              p1.y() + unkown * (p2.y() - p1.y()) + s * (p2.x() - p1.x()));

    // the points that are not shared by the triangles need to be on opposite sites
    if (((((p3prev.x() - p1.x()) * (p2.y() - p1.y()) - (p3prev.y() - p1.y()) * (p2.x() - p1.x()) < 0)
     && ((pu1.x() - p1.x()) * (p2.y() - p1.y()) - (pu1.y() - p1.y()) * (p2.x() - p1.x()) > 0)))
    ||
    (
    (((p3prev.x() - p1.x()) * (p2.y() - p1.y()) - (p3prev.y() - p1.y()) * (p2.x() - p1.x()) > 0)
             && ((pu1.x() - p1.x()) * (p2.y() - p1.y()) - (pu1.y() - p1.y()) * (p2.x() - p1.x()) < 0))
                ))
    {
        pu = pu1;
    }
    else
    {
        pu = pu2;
    }
}

QVector3D Utility::pointToVector(CGAL::Point_3<CGAL::Simple_cartesian<double>>& point)
{
    return QVector3D(float(point.x()), float(point.y()), float(point.z()));
}

float getclockwise(const QVector2D& p)
{
    return -std::atan2(p.x(),-p.y());;
}

bool orderclockwise(const QVector2D& p1, const QVector2D& p2)
{
    return getclockwise(p1) < getclockwise(p2);
}

int isLeftOf(shEdge edge, QVector2D test)
{
    QVector2D tmp1 = edge._from;
    QVector2D tmp2 = edge._to;

    double x = (tmp1.x() * tmp2.y()) - (tmp1.y() * tmp2.x());

    if(x < 0)
    {
        return 0;
    }
    else if (x > 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

bool isNearZero(double testValue)
{
    return std::abs(testValue) <= .000000001;
}

bool isInside(shEdge clipedge, QVector2D point)
{
    int s = isLeftOf(clipedge, point);
    if(s == -1)
    {
        return true;
    }
    else if( s == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool getintersect(QVector2D line1from, QVector2D line1to, QVector2D line2from, QVector2D line2to, QVector2D& out)
{
    QVector2D dir1 = line1to - line1from;
    QVector2D dir2 = line2to - line2from;

    double dotPerp = (dir1.x() * dir2.y()) - (dir1.y() * dir2.x());

    if(isNearZero(dotPerp))
    {
        return false;
    }

    QVector2D c = line2from - line1from;
    double t = (c.x() * dir2.y() - c.y() * dir2.x()) / dotPerp;

    out = line1from + (t * dir1);

    return true;
}

double Utility::intersectionArea(QVector2D& a, QVector2D& b, QVector2D& c, QVector2D& p, QVector2D& q, QVector2D& r)
{
    std::vector<QVector2D> output;
    output.push_back(p);
    output.push_back(q);
    output.push_back(r);
    std::sort(output.begin(), output.end(), orderclockwise);

    std::vector<QVector2D> polylist;
    polylist.push_back(a);
    polylist.push_back(b);
    polylist.push_back(c);
    std::sort(polylist.begin(), polylist.end(), orderclockwise);

    std::vector<shEdge> clipPoly;
    clipPoly.push_back(shEdge(polylist[0], polylist[1]));
    clipPoly.push_back(shEdge(polylist[1], polylist[2]));
    clipPoly.push_back(shEdge(polylist[2], polylist[0]));

    for(shEdge& clipEdge : clipPoly)
    {
        std::vector<QVector2D> inputlist = output;

        if (inputlist.size() == 0)
        {
            break;
        }

        QVector2D S = inputlist[inputlist.size() -1];

        for(QVector2D E : inputlist)
        {
            if(isInside(clipEdge, E))
            {
                if(!isInside(clipEdge, S))
                {
                    QVector2D p;
                    if(getintersect(S,E,clipEdge._from, clipEdge._to, p))
                    {
                       output.push_back(p);
                    }
                }

                output.push_back(E);
            }
            else if (isInside(clipEdge, S))
            {
                QVector2D p;
                if(getintersect(S, E, clipEdge._from, clipEdge._to, p))
                {
                   output.push_back(p);
                }
            }

            S = E;
        }
    }

    double area = 0;

    for(size_t i = 0; i < output.size(); i++)
    {
        size_t j = (i + 1) % output.size();
        area += double((output.at(j).x() + output.at(i).x()) * (output.at(j).y() - output.at(i).y()));
    }

    return std::abs(area / 2.0);
}

float Utility::getcounterclockwise(const QVector2D& p)
{
    return std::atan2(p.x(),-p.y());;
}

bool Utility::compareVector2D(const QVector2D& p1, const QVector2D& p2)
{
    return Utility::getcounterclockwise(p1) < Utility::getcounterclockwise(p2);
}

float Utility::sign(QVector2D& p1, QVector2D& p2, QVector2D& p3)
{
    return (p1.x() - p3.x()) * (p2.y() - p3.y()) - (p2.x() - p3.x()) * (p1.y() - p3.y());
}

void Utility::createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], QOpenGLBuffer& ibo,
                            std::vector<QVector3D>& vertices, std::vector<GLushort>& indices, std::vector<QVector3D>& colors)
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

void Utility::createBuffers(QOpenGLVertexArrayObject& vao, QOpenGLBuffer vbo[], std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors)
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

bool Utility::intersects(QVector2D& p1, QVector2D& q1, QVector2D& p2, QVector2D& q2)
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

int Utility::orientation(QVector2D& p, QVector2D& q, QVector2D& r)
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

bool Utility::onSegment(QVector2D& p, QVector2D& q, QVector2D& r)
{
    if(q.x() <= std::max(p.x(), r.x()) && q.x() >= std::min(p.x(), r.x()) &&
       q.y() <= std::max(p.y(), r.y()) && q.y() >= std::min(p.y(), r.y()))
    {
        return true;
    }

    return false;
}

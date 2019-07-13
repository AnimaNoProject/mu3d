#pragma once
#include <QVector3D>
#include <QVector2D>

#include "gluetag.h"
#include "utility.h"
#include "facetoplane.h"

class Gluetag;
class FaceToPlane;

class GluetagToPlane
{
public:
    GluetagToPlane(Gluetag* gluetag);
    ~GluetagToPlane();

    QVector2D a;
    QVector2D b;
    QVector2D c;
    QVector2D d;

    bool _overlaps;

    long faceindex;
    long self;

    Gluetag* _gluetag;

    double overlaps(GluetagToPlane& other);
    double overlaps(FaceToPlane& other);

    QVector2D const& get(QVector3D const &vec);
    QVector3D const& get(QVector2D const &vec);
    QVector3D const& get(QVector3D const &one, QVector3D const &two);

    void drawproperties(std::vector<QVector3D>& vertices, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colors);
private:
    static QVector3D _colorOverlap;
    static QVector3D _color;
};

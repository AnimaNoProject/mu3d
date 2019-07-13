#pragma once
#include <QVector3D>
#include <QVector2D>

#include "utility.h"

class FaceToPlane
{
public:
    FaceToPlane();
    ~FaceToPlane();

    QVector3D A;
    QVector3D B;
    QVector3D C;

    QVector2D a;
    QVector2D b;
    QVector2D c;

    bool _overlaps;

    long parent;
    long self;

    QVector2D const& get(QVector3D const &vec);
    QVector3D const& get(QVector2D const &vec);
    QVector3D const& get(QVector3D const &one, QVector3D const &two);

    double overlaps(FaceToPlane& other);
    void drawproperties(std::vector<QVector3D>& vertices, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colors);
private:
    static QVector3D _color;
    static QVector3D _colorOverlap;
};

#pragma once
#include <QVector3D>
#include <QVector2D>

class FaceToPlane
{
public:
    QVector3D A;
    QVector3D B;
    QVector3D C;

    QVector2D a;
    QVector2D b;
    QVector2D c;

    int faceId;

    QVector2D const& get(QVector3D const &vec);
    QVector3D const& get(QVector2D const &vec);
    QVector3D const& get(QVector3D const &one, QVector3D const &two);
};

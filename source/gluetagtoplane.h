#pragma once
#include <QVector3D>
#include <QVector2D>

#include "gluetag.h"

class Gluetag;

class GluetagToPlane
{
public:
    GluetagToPlane(Gluetag& gluetag);
    ~GluetagToPlane();

    QVector2D a;
    QVector2D b;
    QVector2D c;
    QVector2D d;

    Gluetag& _gluetag;

    QVector2D const& get(QVector3D const &vec);
    QVector3D const& get(QVector2D const &vec);
    QVector3D const& get(QVector3D const &one, QVector3D const &two);
};

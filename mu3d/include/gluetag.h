#pragma once
#include "edge.h"

class gluetag
{
public:
    gluetag(edge& edge, bool flag);
    int _placedFace;
    int _targetFace;
    edge _edge;
    glm::vec3 _bl;
    glm::vec3 _br;
    glm::vec3 _tl;
    glm::vec3 _tr;

    double _probability;

    bool operator<(const gluetag& other) const;
};

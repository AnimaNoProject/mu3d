#pragma once
#include "edge.h"

namespace mu3d
{
    class gluetab
    {
    public:
        gluetab(edge& edge, bool flag);
        int _placedFace;
        int _targetFace;
        edge _edge;
        glm::dvec3 _bl;
        glm::dvec3 _br;
        glm::dvec3 _tl;
        glm::dvec3 _tr;

        double _probability;

        bool operator<(const gluetab& other) const;
    };
}

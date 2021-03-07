#pragma once
#include <glm.hpp>
#include "gluetab.h"
#include "facetoplane.h"

namespace mu3d
{
    class gluetab;
    class faceToPlane;

    class gluetabToPlane
    {
    public:
        gluetabToPlane(gluetab* gluetab);
        ~gluetabToPlane();

        glm::dvec2 a;
        glm::dvec2 b;
        glm::dvec2 c;
        glm::dvec2 d;

        bool _overlaps;

        long faceindex;
        long self;

        gluetab* _gluetag;

        double overlaps(gluetabToPlane& other);
        double overlaps(faceToPlane& other);

        glm::dvec2 const& get(glm::dvec3 const& vec);
        glm::dvec3 const& get(glm::dvec2 const& vec);
    };
}
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

        glm::vec2 a;
        glm::vec2 b;
        glm::vec2 c;
        glm::vec2 d;

        bool _overlaps;

        long faceindex;
        long self;

        gluetab* _gluetag;

        double overlaps(gluetabToPlane& other);
        double overlaps(faceToPlane& other);

        glm::vec2 const& get(glm::vec3 const& vec);
        glm::vec3 const& get(glm::vec2 const& vec);
    };
}
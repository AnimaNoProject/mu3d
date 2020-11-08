#pragma once
#include <glm.hpp>
#include "gluetag.h"
#include "facetoplane.h"

class gluetag;
class faceToPlane;

class gluetagToPlane
{
public:
    gluetagToPlane(gluetag* gluetag);
    ~gluetagToPlane();

    glm::vec2 a;
    glm::vec2 b;
    glm::vec2 c;
    glm::vec2 d;

    bool _overlaps;

    long faceindex;
    long self;

    gluetag* _gluetag;

    double overlaps(gluetagToPlane& other);
    double overlaps(faceToPlane& other);

    glm::vec2 const& get(glm::vec3 const& vec);
    glm::vec3 const& get(glm::vec2 const& vec);
};

#pragma once
#include <glm.hpp>
#include <vector>
#include "edge.h"

class faceToPlane
{
public:
    faceToPlane();
    ~faceToPlane();

    glm::vec3 A;
    glm::vec3 B;
    glm::vec3 C;

    glm::vec2 a;
    glm::vec2 b;
    glm::vec2 c;

    bool _overlaps;

    long parent;
    long self;

    glm::vec2 const& get(glm::vec3 const& vec);
    glm::vec3 const& get(glm::vec2 const& vec);
    glm::vec3 const& get(glm::vec3 const& one, glm::vec3 const& two);

    double overlaps(faceToPlane& other);
private:
    static glm::vec3 _color;
    static glm::vec3 _colorOverlap;
};

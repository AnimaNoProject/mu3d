#pragma once
#include <glm.hpp>
#include <vector>
#include "edge.h"

namespace mu3d
{
    class faceToPlane
    {
    public:
        faceToPlane();
        ~faceToPlane();

        glm::dvec3 A;
        glm::dvec3 B;
        glm::dvec3 C;

        glm::dvec2 a;
        glm::dvec2 b;
        glm::dvec2 c;

        bool _overlaps;

        long parent;
        long self;

        glm::dvec2 const& get(glm::dvec3 const& vec);
        glm::dvec3 const& get(glm::dvec2 const& vec);
        glm::dvec3 const& get(glm::dvec3 const& one, glm::dvec3 const& two);

        double overlaps(faceToPlane& other);
    private:
        static glm::dvec3 _color;
        static glm::dvec3 _colorOverlap;
    };
}
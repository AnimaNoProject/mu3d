#include <facetoplane.h>
#include <utility.h>

    faceToPlane::~faceToPlane()
    {

    }

    faceToPlane::faceToPlane() : _overlaps(false)
    {
    }

    double faceToPlane::overlaps(faceToPlane& other)
    {
        if (utility::intersects(a, b, other.a, other.b) ||
            utility::intersects(a, b, other.b, other.c) ||
            utility::intersects(a, b, other.c, other.a) ||
            utility::intersects(b, c, other.a, other.b) ||
            utility::intersects(b, c, other.b, other.c) ||
            utility::intersects(b, c, other.c, other.a) ||
            utility::intersects(c, a, other.a, other.b) ||
            utility::intersects(c, a, other.b, other.c) ||
            utility::intersects(c, a, other.c, other.a))
        {
            return utility::intersectionArea(a, b, c, other.a, other.b, other.c);
        }
        else
        {
            return 0;
        }

    }

    glm::vec2 const& faceToPlane::get(glm::vec3 const& vec)
    {
        if (vec == A)
            return a;
        if (vec == B)
            return b;
        if (vec == C)
            return c;
        else
            throw std::invalid_argument("something went wrong trying to retrieve 2D representation");
    }

    glm::vec3 const& faceToPlane::get(glm::vec2 const& vec)
    {
        if (vec == a)
            return A;
        if (vec == b)
            return B;
        if (vec == c)
            return C;
        else
            throw std::invalid_argument("something went wrong trying to retrieve 3D representation");
    }

    glm::vec3 const& faceToPlane::get(glm::vec3 const& one, glm::vec3 const& two)
    {
        if (one != A && two != A)
            return A;
        if (one != B && two != B)
            return B;
        if (one != C && two != C)
            return C;
        else
            throw std::invalid_argument("something went wrong");

    }

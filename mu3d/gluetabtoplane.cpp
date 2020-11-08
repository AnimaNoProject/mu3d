#include "gluetabtoplane.h"
#include <utility.h>
namespace mu3d
{
    gluetabToPlane::gluetabToPlane(gluetab* gluetab) : _gluetag(gluetab)
    {
    }

    gluetabToPlane::~gluetabToPlane()
    {

    }

    double gluetabToPlane::overlaps(gluetabToPlane& other)
    {
        if (utility::intersects(a, c, other.a, other.c)
            || utility::intersects(a, c, other.c, other.d)
            || utility::intersects(a, c, other.b, other.d)

            || utility::intersects(c, d, other.a, other.c)
            || utility::intersects(c, d, other.c, other.d)
            || utility::intersects(c, d, other.b, other.d)

            || utility::intersects(b, d, other.a, other.c)
            || utility::intersects(b, d, other.c, other.d)
            || utility::intersects(b, d, other.b, other.d))
        {
            return utility::intersectionArea(a, b, c, other.a, other.b, other.c) + utility::intersectionArea(c, b, d, other.a, other.b, other.c) +
                utility::intersectionArea(a, b, c, other.c, other.b, other.d) + utility::intersectionArea(c, b, d, other.c, other.b, other.d);
        }
        else
        {
            return 0;
        }
    }

    double gluetabToPlane::overlaps(faceToPlane& other)
    {
        if (utility::intersects(a, c, other.a, other.b)
            || utility::intersects(a, c, other.b, other.c)
            || utility::intersects(a, c, other.c, other.a)

            || utility::intersects(c, d, other.a, other.b)
            || utility::intersects(c, d, other.b, other.c)
            || utility::intersects(c, d, other.c, other.a)

            || utility::intersects(b, d, other.a, other.b)
            || utility::intersects(b, d, other.b, other.c)
            || utility::intersects(b, d, other.c, other.a))
        {
            return utility::intersectionArea(a, b, c, other.a, other.b, other.c) + utility::intersectionArea(c, b, d, other.a, other.b, other.c);
        }
        else
        {
            return 0;
        }

    }

    glm::vec2 const& gluetabToPlane::get(glm::vec3 const& vec)
    {
        if (vec == _gluetag->_bl)
            return a;
        if (vec == _gluetag->_br)
            return b;
        if (vec == _gluetag->_tl)
            return c;
        if (vec == _gluetag->_tr)
            return d;
        throw std::invalid_argument("something went wrong trying to retrieve 2D representation");
    }

    glm::vec3 const& gluetabToPlane::get(glm::vec2 const& vec)
    {
        if (vec == a)
            return _gluetag->_bl;
        if (vec == b)
            return _gluetag->_br;
        if (vec == c)
            return _gluetag->_tl;
        if (vec == d)
            return _gluetag->_tr;

        throw std::invalid_argument("something went wrong trying to retrieve 3D representation");
    }
}
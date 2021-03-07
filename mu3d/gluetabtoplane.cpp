#include "gluetabtoplane.h"
#include <utility.hpp>
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
        return utility::sh_overlapping_area(a, b, c, other.a, other.b, other.c) + utility::sh_overlapping_area(c, b, d, other.a, other.b, other.c) +
            utility::sh_overlapping_area(a, b, c, other.c, other.b, other.d) + utility::sh_overlapping_area(c, b, d, other.c, other.b, other.d);
    }

    double gluetabToPlane::overlaps(faceToPlane& other)
    {
        return utility::sh_overlapping_area(a, b, c, other.a, other.b, other.c) + utility::sh_overlapping_area(c, b, d, other.a, other.b, other.c);
    }

    glm::dvec2 const& gluetabToPlane::get(glm::dvec3 const& vec)
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

    glm::dvec3 const& gluetabToPlane::get(glm::dvec2 const& vec)
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
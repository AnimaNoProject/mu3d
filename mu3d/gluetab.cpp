#include <gluetab.h>

namespace mu3d
{
	gluetab::gluetab(edge& edge, bool flag) : _edge(edge), _probability(0)
	{
		Facet face;

		if (flag)
		{
			_placedFace = edge._sFace;
			_targetFace = edge._tFace;
			face = edge._tFacetHandle;
		}
		else
		{
			_placedFace = edge._tFace;
			_targetFace = edge._sFace;
			face = edge._sFacetHandle;
		}

		// get the bottom left corner of the base
		_bl = glm::dvec3(_edge._halfedge->vertex()->point().x(),
			_edge._halfedge->vertex()->point().y(),
			_edge._halfedge->vertex()->point().z());

		// get the bottom right corner of the base
		_br = glm::dvec3(_edge._halfedge->prev()->vertex()->point().x(),
			_edge._halfedge->prev()->vertex()->point().y(),
			_edge._halfedge->prev()->vertex()->point().z());

		// get the vertex where the gluetab is extruded to
		glm::dvec3 target;
		Polyhedron::Halfedge_around_facet_circulator hfc = face->facet_begin();
		do
		{
			target = glm::dvec3(hfc->vertex()->point().x(),
				hfc->vertex()->point().y(),
				hfc->vertex()->point().z());

			// if the vertex is neither the bottom right or bottom left it is the target
			if (target != _bl && target != _br)
			{
				break;
			}
		} while (++hfc != face->facet_begin());


		glm::dvec3 side = (_br - _bl) / 8.0;

		_tr = _br + (target - _br) / 3.0 - side * 1.8;
		_tl = _bl + (target - _bl) / 3.0 + side * 1.8;

		_br = _br - side;
		_bl = _bl + side;
	}

	bool gluetab::operator<(const gluetab& other) const
	{
		return _probability < other._probability;
	}
}
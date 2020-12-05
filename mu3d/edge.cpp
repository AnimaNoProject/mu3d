#include <edge.h>
#include <utility.hpp>

namespace mu3d
{
	edge::edge(int sFace, int tFace, glm::vec3 middle, Halfedge halfedge, Facet sFacetHandle, Facet tFacetHandle)
		: _sFace(sFace), _tFace(tFace),
		_sFacetHandle(sFacetHandle), _tFacetHandle(tFacetHandle), _halfedge(halfedge), _weight(double(std::rand()) / RAND_MAX)
	{

		/*
		 * Calculate if the edge is bent inwards or outwards.
		 * */
		isInwards = true;

		glm::vec3 A = utility::point_to_vector(sFacetHandle->halfedge()->vertex()->point());
		glm::vec3 B = utility::point_to_vector(sFacetHandle->halfedge()->next()->vertex()->point());
		glm::vec3 C = utility::point_to_vector(sFacetHandle->halfedge()->next()->next()->vertex()->point());

		glm::vec3 F = utility::point_to_vector(tFacetHandle->halfedge()->vertex()->point());
		glm::vec3 G = utility::point_to_vector(tFacetHandle->halfedge()->next()->vertex()->point());
		glm::vec3 H = utility::point_to_vector(tFacetHandle->halfedge()->next()->next()->vertex()->point());

		glm::vec3 p;
		glm::vec3 q;

		if (A != F && A != G && A != H)
		{
			p = A;
			q = B;
		}
		else if (B != F && B != G && B != H)
		{
			p = B;
			q = C;
		}
		else if (C != F && C != G && C != H)
		{
			p = C;
			q = A;
		}

		glm::vec3 e = p - q;
		glm::vec3 n = glm::cross(G - F, H - G);
		float angle = glm::dot(e, n);
		if (angle <= 0)
		{
			isInwards = false;
		}
	}

	edge::edge()
	{

	}

	edge::~edge()
	{
	}

	bool edge::isNeighbour(const edge& edge)
	{
		return _halfedge->vertex() == edge._halfedge->vertex()
			|| _halfedge->prev()->vertex() == edge._halfedge->vertex()
			|| _halfedge->vertex() == edge._halfedge->prev()->vertex()
			|| _halfedge->prev()->vertex() == edge._halfedge->prev()->vertex();
	}

	bool edge::is(int A, int B)
	{
		return (_sFace == A && _tFace == B) || (_sFace == B && _tFace == A);
	}

	bool edge::operator==(const edge& other) const
	{
		return (_sFace == other._sFace && _tFace == other._tFace) ||
			(_sFace == other._tFace && _tFace == other._sFace);
	}

	bool edge::operator<(const edge& other) const
	{
		return _weight < other._weight;
	}
}
#pragma once

#include <glm.hpp>

#include <fstream>
#include <iostream>
#include <list>

#include <math.h>

#include "graph.h"

struct shEdge
{
public:

	shEdge(glm::vec2& from, glm::vec2& to)
	{
		_from = from;
		_to = to;
	}

	glm::vec2 _from;
	glm::vec2 _to;
};

class utility
{
public:
	static glm::vec3 point_to_vector(CGAL::Point_3<CGAL::Simple_cartesian<double>>& point);

	static bool intersects(glm::vec2& p1, glm::vec2& q1, glm::vec2& p2, glm::vec2& q2);
	static double intersectionArea(glm::vec2& p1, glm::vec2& q1, glm::vec2& r1, glm::vec2& p2, glm::vec2& q2, glm::vec2& r2);

	static void planar(glm::vec3 const& A, glm::vec3 const& B, glm::vec3 const& C, glm::vec2& a, glm::vec2& b, glm::vec2& c);
	static void planar(glm::vec3 const& P1, glm::vec3 const& P2, glm::vec3 const& Pu, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3prev, glm::vec2& pu);
	static void gtMirror(glm::vec3 const& P1, glm::vec3 const& P2, glm::vec3 const& Pu, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3prev, glm::vec2& pu);
private:
	static int orientation(glm::vec2& p, glm::vec2& q, glm::vec2& r);
	static bool onSegment(glm::vec2& p, glm::vec2& q, glm::vec2& r);
	static float sign(glm::vec2& p1, glm::vec2& p2, glm::vec2& p3);

	static bool compareVector2D(const glm::vec2& p1, const glm::vec2& p2);
	static float getcounterclockwise(const glm::vec2& p);
};

#include "utility.h"

void utility::planar(glm::vec3 const& A, glm::vec3 const& B, glm::vec3 const& C, glm::vec2& a, glm::vec2& b, glm::vec2& c)
{
	
	float lengthAB = glm::length(A - B);

	a = glm::vec2(0, 0);
	b = glm::vec2(lengthAB, 0);

	float s = glm::length(glm::cross(B - A, C - A)) / float(std::pow(lengthAB, 2));
	float cl = glm::dot(B - A, C - A) / float(std::pow(lengthAB, 2));

	c = glm::vec2(a.x + cl * (b.x - a.x) - s * (b.y - a.y),
		a.y + cl * (b.y - a.y) + s * (b.x - a.x));
}

void utility::planar(glm::vec3 const& P1, glm::vec3 const& P2, glm::vec3 const& Pu, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3prev, glm::vec2& pu)
{
	float length = glm::length(p1 - p2);

	float s = glm::length(glm::cross((P2 - P1), (Pu - P1))) / float(std::pow(length, 2));
	float unkown = glm::dot((P2 - P1), (Pu - P1)) / float(std::pow(length, 2));

	glm::vec2 pu1 = glm::vec2(p1.x + unkown * (p2.x - p1.x) + s * (p2.y - p1.y),
		p1.y + unkown * (p2.y - p1.y) - s * (p2.x - p1.x));

	glm::vec2 pu2 = glm::vec2(p1.x + unkown * (p2.x - p1.x) - s * (p2.y - p1.y),
		p1.y + unkown * (p2.y - p1.y) + s * (p2.x - p1.x));

	// the points that are not shared by the triangles need to be on opposite sites
	if (((((p3prev.x - p1.x) * (p2.y - p1.y) - (p3prev.y - p1.y) * (p2.x - p1.x) < 0)
		&& ((pu1.x - p1.x) * (p2.y - p1.y) - (pu1.y - p1.y) * (p2.x - p1.x) > 0)))
		||
		(
			(((p3prev.x - p1.x) * (p2.y - p1.y) - (p3prev.y - p1.y) * (p2.x - p1.x) > 0)
				&& ((pu1.x - p1.x) * (p2.y - p1.y) - (pu1.y - p1.y) * (p2.x - p1.x) < 0))
			))
	{
		pu = pu1;
	}
	else
	{
		pu = pu2;
	}
}

void utility::gtMirror(glm::vec3 const& P1, glm::vec3 const& P2, glm::vec3 const& Pu, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3prev, glm::vec2& pu)
{
	float length = glm::length(p1 - p2);

	float s = glm::length(glm::cross((P2 - P1), (Pu - P1))) / float(std::pow(length, 2));
	float unkown = glm::dot((P2 - P1), (Pu - P1)) / float(std::pow(length, 2));

	glm::vec2 pu1 = glm::vec2(p1.x + unkown * (p2.x - p1.x) + s * (p2.y - p1.y),
		p1.y + unkown * (p2.y - p1.y) - s * (p2.x - p1.x));

	glm::vec2 pu2 = glm::vec2(p1.x + unkown * (p2.x - p1.x) - s * (p2.y - p1.y),
		p1.y + unkown * (p2.y - p1.y) + s * (p2.x - p1.x));

	// the points that are not shared by the triangles need to be on opposite sites
	if (((((p3prev.x - p1.x) * (p2.y - p1.y) - (p3prev.y - p1.y) * (p2.x - p1.x) < 0)
		&& ((pu1.x - p1.x) * (p2.y - p1.y) - (pu1.y - p1.y) * (p2.x - p1.x) > 0)))
		||
		(
			(((p3prev.x - p1.x) * (p2.y - p1.y) - (p3prev.y - p1.y) * (p2.x - p1.x) > 0)
				&& ((pu1.x - p1.x) * (p2.y - p1.y) - (pu1.y - p1.y) * (p2.x - p1.x) < 0))
			))
	{
		pu = pu2;
	}
	else
	{
		pu = pu1;
	}
}

glm::vec3 utility::point_to_vector(CGAL::Point_3<CGAL::Simple_cartesian<double>>& point)
{
	return glm::vec3(float(point.x()), float(point.y()), float(point.z()));
}

float getclockwise(const glm::vec2& p)
{
	return -std::atan2(p.x, -p.y);;
}

bool orderclockwise(const glm::vec2& p1, const glm::vec2& p2)
{
	return getclockwise(p1) < getclockwise(p2);
}

int isLeftOf(shEdge edge, glm::vec2 test)
{
	glm::vec2 tmp1 = edge._from;
	glm::vec2 tmp2 = edge._to;

	float x = (tmp1.x * tmp2.y) - (tmp1.y * tmp2.x);

	if (x < 0)
	{
		return 0;
	}
	else if (x > 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

bool isNearZero(double testValue)
{
	return std::abs(testValue) <= .000000001;
}

bool isInside(shEdge clipedge, glm::vec2 point)
{
	int s = isLeftOf(clipedge, point);
	if (s == -1)
	{
		return true;
	}
	else if (s == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool getintersect(glm::vec2 line1from, glm::vec2 line1to, glm::vec2 line2from, glm::vec2 line2to, glm::vec2& out)
{
	glm::vec2 dir1 = line1to - line1from;
	glm::vec2 dir2 = line2to - line2from;

	float dotPerp = float((dir1.x * dir2.y)) - float((dir1.y * dir2.x));

	if (isNearZero(dotPerp))
	{
		return false;
	}

	glm::vec2 c = line2from - line1from;
	float t = float((c.x * dir2.y - c.y * dir2.x)) / dotPerp;

	out = line1from + (t * dir1);

	return true;
}

double utility::intersectionArea(glm::vec2& a, glm::vec2& b, glm::vec2& c, glm::vec2& p, glm::vec2& q, glm::vec2& r)
{
	std::vector<glm::vec2> output;
	output.push_back(p);
	output.push_back(q);
	output.push_back(r);
	std::sort(output.begin(), output.end(), orderclockwise);

	std::vector<glm::vec2> polylist;
	polylist.push_back(a);
	polylist.push_back(b);
	polylist.push_back(c);
	std::sort(polylist.begin(), polylist.end(), orderclockwise);

	std::vector<shEdge> clipPoly;
	clipPoly.push_back(shEdge(polylist[0], polylist[1]));
	clipPoly.push_back(shEdge(polylist[1], polylist[2]));
	clipPoly.push_back(shEdge(polylist[2], polylist[0]));

	for (shEdge& clipEdge : clipPoly)
	{
		std::vector<glm::vec2> inputlist = output;

		if (inputlist.size() == 0)
		{
			break;
		}

		glm::vec2 S = inputlist[inputlist.size() - 1];

		for (glm::vec2 E : inputlist)
		{
			if (isInside(clipEdge, E))
			{
				if (!isInside(clipEdge, S))
				{
					glm::vec2 p;
					if (getintersect(S, E, clipEdge._from, clipEdge._to, p))
					{
						output.push_back(p);
					}
				}

				output.push_back(E);
			}
			else if (isInside(clipEdge, S))
			{
				glm::vec2 p;
				if (getintersect(S, E, clipEdge._from, clipEdge._to, p))
				{
					output.push_back(p);
				}
			}

			S = E;
		}
	}

	double area = 0;

	for (size_t i = 0; i < output.size(); i++)
	{
		size_t j = (i + 1) % output.size();
		area += double((output.at(j).x + output.at(i).x) * (output.at(j).y - output.at(i).y));
	}

	return std::abs(area / 2.0);
}

float utility::getcounterclockwise(const glm::vec2& p)
{
	return std::atan2(p.x, -p.y);;
}

bool utility::compareVector2D(const glm::vec2& p1, const glm::vec2& p2)
{
	return utility::getcounterclockwise(p1) < utility::getcounterclockwise(p2);
}

float utility::sign(glm::vec2& p1, glm::vec2& p2, glm::vec2& p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool utility::intersects(glm::vec2& p1, glm::vec2& q1, glm::vec2& p2, glm::vec2& q2)
{
	if ((p1 == p2 && q1 != q2) || (p1 == q2 && q1 != p2) || (q1 == p2 && p1 != q2) || (q1 == q2 && p1 != p2))
	{
		return false;
	}

	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	if (o1 != o2 && o3 != o4)
	{
#ifndef NDEBUG
		std::cout << "p1 = [" << p1.x << "," << p1.y << "]"
			<< "q1 = [" << q1.x << "," << q1.y << "]" << std::endl
			<< "p2 = [" << p2.x << "," << p2.y << "]"
			<< "q2 = [" << q2.x << "," << q2.y << "]" << std::endl << std::endl;
#endif
		return true;
	}

	if ((o1 == 0 && onSegment(p1, p2, q1))
		|| (o2 == 0 && onSegment(p1, q2, q1))
		|| (o3 == 0 && onSegment(p2, p1, q2))
		|| (o4 == 0 && onSegment(p2, q1, q2)))
	{
#ifndef NDEBUG
		std::cout << "onSegment && o1/4 = 0" << std::endl;
#endif
		return true;
	}

	return false;
}

int utility::orientation(glm::vec2& p, glm::vec2& q, glm::vec2& r)
{
	float val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0)
	{
		return 0;
	}
	else if (val > 0)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

bool utility::onSegment(glm::vec2& p, glm::vec2& q, glm::vec2& r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
	{
		return true;
	}

	return false;
}

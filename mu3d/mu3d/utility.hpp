#pragma once

#include <glm.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <math.h>
#include <cmath>
#include "graph.h"

namespace mu3d
{
	class utility
	{
	public:
		static glm::dvec3 point_to_vector(CGAL::Point_3<CGAL::Simple_cartesian<double>>& point)
		{
			return glm::dvec3(point.x(), point.y(), point.z());
		}
		static void planar(glm::dvec3 const& A, glm::dvec3 const& B, glm::dvec3 const& C, glm::dvec2& a, glm::dvec2& b, glm::dvec2& c)
		{

			double lengthAB = glm::length(A - B);

			a = glm::dvec2(0, 0);
			b = glm::dvec2(lengthAB, 0);

			double s = glm::length(glm::cross(B - A, C - A)) / std::pow(lengthAB, 2);
			double cl = glm::dot(B - A, C - A) / std::pow(lengthAB, 2);

			c = glm::dvec2(a.x + cl * (b.x - a.x) - s * (b.y - a.y),
				a.y + cl * (b.y - a.y) + s * (b.x - a.x));
		}
		static void planar(glm::dvec3 const& P1, glm::dvec3 const& P2, glm::dvec3 const& Pu, glm::dvec2 const& p1, glm::dvec2 const& p2, glm::dvec2 const& p3prev, glm::dvec2& pu)
		{
			double length = glm::length(p1 - p2);

			double s = glm::length(glm::cross((P2 - P1), (Pu - P1))) / std::pow(length, 2);
			double unkown = glm::dot((P2 - P1), (Pu - P1)) / std::pow(length, 2);

			glm::dvec2 pu1 = glm::dvec2(p1.x + unkown * (p2.x - p1.x) + s * (p2.y - p1.y),
				p1.y + unkown * (p2.y - p1.y) - s * (p2.x - p1.x));

			glm::dvec2 pu2 = glm::dvec2(p1.x + unkown * (p2.x - p1.x) - s * (p2.y - p1.y),
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
		static void print_progress(int block_idx, double energy)
		{
			for (int i = 0; i < block_idx; i++)
			{
				std::cout << "[]";
			}

			for (int i = block_idx; i < 10; i++)
			{
				std::cout << "_";
			}

			std::cout << " " << block_idx / 10.0f * 100 << "% with " << energy << std::endl;
		}
		static double sh_overlapping_area(glm::dvec2& a, glm::dvec2& b, glm::dvec2& c, glm::dvec2& p, glm::dvec2& q, glm::dvec2& r)
		{
			std::vector<glm::dvec2> subject_polygon = { a, b, c };
			std::vector<glm::dvec2> new_polygon = { a, b, c };
			std::vector<glm::dvec2> input_polygon;
			std::vector<glm::dvec2> clipper = { p, q, r };

			for (size_t j = 0; j < clipper.size(); j++)
			{
				input_polygon.clear();
				for (size_t k = 0; k < new_polygon.size(); k++)
				{
					input_polygon.push_back(new_polygon[k]);
				}
				new_polygon.clear();

				glm::dvec2 cp1 = clipper[j];
				glm::dvec2 cp2 = clipper[(j + 1) % 3];

				for (int i = 0; i < input_polygon.size(); i++)
				{
					glm::dvec2 s = input_polygon[i];
					glm::dvec2 e = input_polygon[(i + 1) % input_polygon.size()];

					// both vertex inside
					if (inside(s, cp1, cp2) && inside(e, cp1, cp2))
					{
						new_polygon.push_back(e);
					}
					// first vertex outside, second inside
					else if (!inside(s, cp1, cp2) && inside(e, cp1, cp2))
					{
						new_polygon.push_back(intersection(cp1, cp2, s, e));
						new_polygon.push_back(e);
					}
					// first vertex inside, second one outside
					else if (inside(s, cp1, cp2) && !inside(e, cp1, cp2))
					{
						new_polygon.push_back(intersection(cp1, cp2, s, e));
					}
					// else both vertices outside
				}
			}

			std::sort(new_polygon.begin(), new_polygon.end(), [](const glm::dvec2& p1, const glm::dvec2& p2){ return -std::atan2(p1.x, -p1.y) < -std::atan2(p2.x, -p2.y); });

			double leftSum = 0.0;
			double rightSum = 0.0;

			for (int i = 0; i < new_polygon.size(); i++)
			{
				int j = (i + 1) % new_polygon.size();
				leftSum += new_polygon[i].x * new_polygon[j].y;
				rightSum += new_polygon[j].x * new_polygon[i].y;
			}

			double area = 0.5f * glm::abs(leftSum - rightSum);
			if (area < 1e-5) // eliminate the cases where two triangles share one vertex
			{
				return 0;
			}
			return area;
		}
	private:
		static bool inside(glm::dvec2& p, glm::dvec2& p1, glm::dvec2& p2)
		{
			return (p2.y - p1.y) * p.x + (p1.x - p2.x) * p.y + (p2.x * p1.y - p1.x * p2.y) < 0;
		}
		static glm::dvec2 intersection(glm::dvec2& cp1, glm::dvec2& cp2, glm::dvec2& s, glm::dvec2& e)
		{
			glm::dvec2 dc = { cp1.x - cp2.x, cp1.y - cp2.y };
			glm::dvec2 dp = { s.x - e.x, s.y - e.y };

			double n1 = cp1.x * cp2.y - cp1.y * cp2.x;
			double n2 = s.x * e.y - s.y * e.x;
			double n3 = 1.0f / (dc.x * dp.y - dc.y * dp.x);

			return { (n1 * dp.x - n2 * dc.x) * n3, (n1 * dp.y - n2 * dc.y) * n3 };
		}
	};
}
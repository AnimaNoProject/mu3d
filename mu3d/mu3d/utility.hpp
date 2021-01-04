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
		static glm::vec3 point_to_vector(CGAL::Point_3<CGAL::Simple_cartesian<double>>& point)
		{
			return glm::vec3(float(point.x()), float(point.y()), float(point.z()));
		}
		static void planar(glm::vec3 const& A, glm::vec3 const& B, glm::vec3 const& C, glm::vec2& a, glm::vec2& b, glm::vec2& c)
		{

			float lengthAB = glm::length(A - B);

			a = glm::vec2(0, 0);
			b = glm::vec2(lengthAB, 0);

			float s = glm::length(glm::cross(B - A, C - A)) / float(std::pow(lengthAB, 2));
			float cl = glm::dot(B - A, C - A) / float(std::pow(lengthAB, 2));

			c = glm::vec2(a.x + cl * (b.x - a.x) - s * (b.y - a.y),
				a.y + cl * (b.y - a.y) + s * (b.x - a.x));
		}
		static void planar(glm::vec3 const& P1, glm::vec3 const& P2, glm::vec3 const& Pu, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3prev, glm::vec2& pu)
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

			std::cout << " " << block_idx / 10.0f * 100 << "%" << std::endl;
		}
		static double sh_overlapping_area(glm::vec2& a, glm::vec2& b, glm::vec2& c, glm::vec2& p, glm::vec2& q, glm::vec2& r)
		{
			std::vector<glm::vec2> subject_polygon = { a, b, c };
			std::vector<glm::vec2> new_polygon = { a, b, c };
			std::vector<glm::vec2> input_polygon;
			std::vector<glm::vec2> clipper = { p, q, r };

			for (int j = 0; j < clipper.size(); j++)
			{
				input_polygon.clear();
				for (int k = 0; k < new_polygon.size(); k++)
				{
					input_polygon.push_back(new_polygon[k]);
				}
				new_polygon.clear();

				glm::vec2 cp1 = clipper[j];
				glm::vec2 cp2 = clipper[(j + 1) % 3];

				for (int i = 0; i < input_polygon.size(); i++)
				{
					glm::vec2 s = input_polygon[i];
					glm::vec2 e = input_polygon[(i + 1) % input_polygon.size()];

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

			std::sort(new_polygon.begin(), new_polygon.end(), [](const glm::vec2& p1, const glm::vec2& p2){ return -std::atan2(p1.x, -p1.y) < -std::atan2(p2.x, -p2.y); });

			float leftSum = 0.0;
			float rightSum = 0.0;

			for (int i = 0; i < new_polygon.size(); i++)
			{
				int j = (i + 1) % new_polygon.size();
				leftSum += new_polygon[i].x * new_polygon[j].y;
				rightSum += new_polygon[j].x * new_polygon[i].y;
			}

			float area = 0.5f * glm::abs(leftSum - rightSum);
			if (area < 1e-5) // eliminate the cases where two triangles share one vertex
			{
				return 0;
			}
			return area;
		}
	private:
		static bool inside(glm::vec2& p, glm::vec2& p1, glm::vec2& p2)
		{
			return (p2.y - p1.y) * p.x + (p1.x - p2.x) * p.y + (p2.x * p1.y - p1.x * p2.y) < 0;
		}
		static glm::vec2 intersection(glm::vec2& cp1, glm::vec2& cp2, glm::vec2& s, glm::vec2& e)
		{
			glm::vec2 dc = { cp1.x - cp2.x, cp1.y - cp2.y };
			glm::vec2 dp = { s.x - e.x, s.y - e.y };

			float n1 = cp1.x * cp2.y - cp1.y * cp2.x;
			float n2 = s.x * e.y - s.y * e.x;
			float n3 = 1.0f / (dc.x * dp.y - dc.y * dp.x);

			return { (n1 * dp.x - n2 * dc.x) * n3, (n1 * dp.y - n2 * dc.y) * n3 };
		}
	};
}
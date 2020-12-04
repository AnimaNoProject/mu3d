#include <fstream>
#include <graph.h>
#include <utility.h>

namespace mu3d
{
	graph::graph() : _Cenergy(0), _maxtemp(0), 
					_optEnergy(0), _optimise(false), 
					_opttemperature(0), _temperature(0)
	{
	}

	void graph::load(std::string file)
	{
		// open file buffer
		std::filebuf filebuffer;
		if (filebuffer.open(file, std::ios::in))
		{
			// read .off file into CGAL::Polyhedron_3
			std::istream is(&filebuffer);
			CGAL::read_off(is, _mesh);
			filebuffer.close();
		}
		else
		{
			throw std::exception(("Failed to load \"" + file + "\".").c_str());
		}

		// get all vertices
		for (Polyhedron::Vertex_iterator v = _mesh.vertices_begin(); v != _mesh.vertices_end(); ++v)
		{
			_vertices.push_back(utility::point_to_vector(v->point()));
		}

		// loop through all facets
		for (Polyhedron::Facet_iterator fi = _mesh.facets_begin(); fi != _mesh.facets_end(); ++fi)
		{
			if (_facets.empty())
			{
				// first facet
				_facets.insert(std::make_pair(0, fi));
			}
			else
			{
				// all other facets get the index+1
				_facets.insert(std::make_pair(_facets.rbegin()->first + 1, fi));
			}

			// get the first facet
			Polyhedron::Halfedge_around_facet_circulator hfc = fi->facet_begin();
			// assert that all facets are triangles
			CGAL_assertion(CGAL::circulator_size(hfc) >= 3);
			do
			{
				unsigned short foundindex = 0;
				// loop through vertices and save their index if they match
				for (std::vector<int>::size_type i = 0; i != _vertices.size(); ++i)
				{
					if (utility::point_to_vector(hfc->vertex()->point()) == _vertices[i])
					{
						foundindex = short(i);
						break;
					}
				}
				_indices.push_back(foundindex);

			} while (++hfc != fi->facet_begin());
		}
	}

	bool graph::unfold(float max_its, float opt_its)
	{
		initialise(max_its, opt_its);

		while (_temperature > 0 && _Cenergy > 0)
		{
			next();
		}

		while (_opttemperature > 0)
		{
			compactIt();
		}

		return _Cenergy == 0;
	}

	void graph::initialise(float max_its, float opt_its)
	{
		initEdgeWeight();

		// calculate the dualgraph and an initial MSP and Gluetags
		compute_dual();
		compute_mst();
		calculateGlueTags(_gluetags);

		_temperature = max_its;
		_maxtemp = max_its;
		_opttemperature = opt_its;
		_optimise = opt_its == 0;

		// initialize the energy with this unfolding
		unfoldTriangles();
		unfoldGluetags();

		// it is the best we have
		_Cgt = _gluetags;
		_C = _edges;
		_Cenergy = findTriangleOverlaps() + findGluetagOverlaps();
		_CplanarFaces = _planarFaces;
		_CplanarGluetags = _planarGluetags;
	}

	void graph::next()
	{
		randomMove();

		// calculate a new spanning tree and gluetags
		compute_mst();
		calculateGlueTags(_gluetags);

		// unfold and check for overlaps
		unfoldTriangles();
		double trioverlaps = findTriangleOverlaps();
		double gtoverlaps = 0;

		if (trioverlaps <= 0)
		{
			unfoldGluetags();
			gtoverlaps = findGluetagOverlaps();
		}
		else {
			gtoverlaps = 40;
		}

		double newEnergy = trioverlaps + gtoverlaps;

		double chance = (1 - std::pow(std::exp(1), -(_temperature) / _maxtemp)) / 2000 / 10;
		double random = (double(std::rand()) / RAND_MAX);

		// if it got better we take the new graph
		if (newEnergy <= _Cenergy)
		{
			_Cgt.clear();
			_C.clear();

			_Cgt = _gluetags;
			_C = _edges;
			_Cenergy = newEnergy;

			_CplanarFaces.clear();
			_CplanarGluetags.clear();

			_CplanarFaces = _planarFaces;
			_CplanarGluetags = _planarGluetags;
		}
		// if it is worse, there is a chance we take the worse one (helps getting out of local minimum
		else if (chance >= random)
		{
			_Cgt.clear();
			_C.clear();

			_Cgt = _gluetags;
			_C = _edges;
			_Cenergy = newEnergy;

			_CplanarFaces.clear();
			_CplanarGluetags.clear();

			_CplanarFaces = _planarFaces;
			_CplanarGluetags = _planarGluetags;
		}
		// continue working with the best
		else
		{
			_edges.clear();
			_gluetags.clear();

			_edges = _C;
			_gluetags = _Cgt;
		}

		if (_Cenergy <= 0)
		{
			_optimise = true;
			_optEnergy = compactness();
		}

		// end epoch
		_temperature -= 1;
	}

	void graph::compactIt()
	{
		randomMove();

		// calculate a new spanning tree and gluetags
		compute_mst();
		calculateGlueTags(_gluetags);

		// unfold and check for overlaps
		unfoldTriangles();
		double trioverlaps = findTriangleOverlaps();
		double gtoverlaps = 0;

		if (trioverlaps <= 0)
		{
			unfoldGluetags();
			gtoverlaps = findGluetagOverlaps();

			if (gtoverlaps > 0)
			{
				_edges.clear();
				_gluetags.clear();

				_edges = _C;
				_gluetags = _Cgt;
			}
		}
		else {
			_edges.clear();
			_gluetags.clear();

			_edges = _C;
			_gluetags = _Cgt;
		}

		float newEnergy = compactness();
		// if it got better we take the new graph
		if (newEnergy <= _optEnergy)
		{
			_Cgt.clear();
			_C.clear();

			_Cgt = _gluetags;
			_C = _edges;
			_optEnergy = newEnergy;

			_CplanarFaces.clear();
			_CplanarGluetags.clear();

			_CplanarFaces = _planarFaces;
			_CplanarGluetags = _planarGluetags;
		}
		// continue working with the best
		else
		{
			_edges.clear();
			_gluetags.clear();

			_edges = _C;
			_gluetags = _Cgt;
		}

		// end epoch
		_opttemperature -= 1;
	}

	void graph::save(std::string mainmodel, std::string gluetabs)
	{
		std::ofstream transfer(mainmodel);
		transfer << "o Model" << std::endl;
		std::stringstream vs;
		std::stringstream vts;
		std::stringstream fs;
		std::stringstream ns;

		int index = 0;
		for (auto& f2p : _CplanarFaces)
		{
			index += 3;
			vs << "v "		<< f2p.A.x		<< " " << f2p.A.y		<< " "	<< f2p.A.z << std::endl;
			vs << "v "		<< f2p.B.x		<< " " << f2p.B.y		<< " "	<< f2p.B.z << std::endl;
			vs << "v "		<< f2p.C.x		<< " " << f2p.C.y		<< " "	<< f2p.C.z << std::endl;

			fs << "f "		<< index - 2	<< "/" << index - 2		<< "/0" << " "
							<< index - 1	<< "/" << index - 1		<< "/0" << " "
							<< index		<< "/" << index			<< "/0" << std::endl;

			vts << "vt "	<< f2p.a.x		<< " " << f2p.a.y		<< std::endl;
			vts << "vt "	<< f2p.b.x		<< " " << f2p.b.y		<< std::endl;
			vts << "vt "	<< f2p.c.x		<< " " << f2p.c.y		<< std::endl;
		}

		transfer << vs.str();
		transfer << ns.str();
		transfer << vts.str();
		transfer << "s off" << std::endl;
		transfer << fs.str();

		transfer.close();

		std::ofstream transferGT(gluetabs);
		transferGT << "o Gluetabs" << std::endl;
		std::stringstream vsGt;
		std::stringstream fsGt;
		index = 0;
		for (auto& f2p : _CplanarGluetags)
		{
			index += 4;
			vsGt << "v " << f2p.a.x << " " << f2p.a.y << " " << 0 << std::endl;
			vsGt << "v " << f2p.b.x << " " << f2p.b.y << " " << 0 << std::endl;
			vsGt << "v " << f2p.c.x << " " << f2p.c.y << " " << 0 << std::endl;
			//vsGt << "v " << f2p.b.x << " " << f2p.b.y << " " << 0 <<  std::endl;
			vsGt << "v " << f2p.d.x << " " << f2p.d.y << " " << 0 << std::endl;

			fsGt << "f " << index - 3 << "//0 " << index - 2 << "//0 " << index - 1 << "//0" << std::endl;
			fsGt << "f " << index - 1 << "//0 " << index - 2 << "//0 " << index << "//0" << std::endl;
		}

		transferGT << vsGt.str() << std::endl;
		transferGT << fsGt.str() << std::endl;
		transferGT.close();
	}

	void graph::compute_dual()
	{
		// loop through all faces
		for (std::pair<int, Facet> facet : _facets)
		{
			int faceId = facet.first;

			// loop through halfedges of all faces and add the dual edges, that were not added yet
			// use distance as weight
			Polyhedron::Halfedge_around_facet_circulator hfc = facet.second->facet_begin();
			do
			{
				// get the opposing face
				int oppositeFaceId = find(hfc->opposite()->facet());

				// center of the edge
				glm::vec3 center = (utility::point_to_vector(hfc->prev()->vertex()->point()) + utility::point_to_vector(hfc->vertex()->point())) / 2.0f;

				edge e = edge(faceId, oppositeFaceId, center, hfc, _facets[faceId], _facets[oppositeFaceId]);

				// if this edge doesn't exist already, add it (don't consider direction)
				if (!find(e))
				{
					_edges.push_back(e);
				}

			} while (++hfc != facet.second->facet_begin());
		}

		// calculate all possible gluetags
		for (edge& e : _edges)
		{
			gluetab gt = gluetab(e, true);
			_gluetags.push_back(gt);
			gt = gluetab(e, false);
			_gluetags.push_back(gt);
		}
	}

	void graph::compute_mst()
	{
		// clear the previous msp edges
		_mspEdges.clear();
		_cutEdges.clear();

		// sort edges from smallest to biggest, better cut big ones
		std::sort(_edges.begin(), _edges.end());

		// create the adjacence list
		std::vector<std::vector<int>> adjacenceList;
		adjacenceList.resize(_facets.size());

		// go through all possible edges
		for (edge& edge : _edges)
		{
			// add edge to msp, add adjacent faces
			_mspEdges.push_back(edge);
			adjacenceList[size_t(edge._sFace)].push_back(edge._tFace);
			adjacenceList[size_t(edge._tFace)].push_back(edge._sFace);

			// list storing discovered nodes
			std::vector<bool> discovered(_facets.size());

			// if the MSP is now cyclic, the added egde needs to be removed again
			for (int i = 0; i < int(_facets.size()); i++)
			{
				// if the node is alone (no incident edges), or already discovered, no need to check
				if (adjacenceList[size_t(i)].empty() || discovered[size_t(i)])
					continue;

				// if the graph is cyclic
				if (!isAcyclic(adjacenceList, i, discovered, -1))
				{
					_mspEdges.erase(remove(_mspEdges.begin(), _mspEdges.end(), edge), _mspEdges.end());

					// add edge to the "to be cut" list
					_cutEdges.push_back(edge);

					// cleanup the adjacence list
					adjacenceList[size_t(edge._sFace)].erase(remove(adjacenceList[size_t(edge._sFace)].begin(), adjacenceList[size_t(edge._sFace)].end(), edge._tFace), adjacenceList[size_t(edge._sFace)].end());
					adjacenceList[size_t(edge._tFace)].erase(remove(adjacenceList[size_t(edge._tFace)].begin(), adjacenceList[size_t(edge._tFace)].end(), edge._sFace), adjacenceList[size_t(edge._tFace)].end());

					// no need to continue checking
					break;
				}
			}
		}

#ifndef NDEBUG
		// show the number of faces and edgesd::endl;
		std::cout << "number of faces: " << _facets.size() << std::endl;
		std::cout << "number of edges: " << _mspEdges.size() << std::endl;

		// show all edges of the MSP
		std::cout << "MSP over the edges" << std::endl;
		for (edge& edge : _mspEdges)
			std::cout << "Edge: " << edge._sFace << "<->" << edge._tFace << std::endl;

		// check if the graph is a single component
		if (isSingleComponent(adjacenceList))
			std::cout << "Graph is a single component!" << std::endl;
		else
			std::cout << "Graph is a NOT single component!" << std::endl;
#endif
	}

	void graph::calculateGlueTags(std::vector<gluetab> gluetabs)
	{
		// clear old gluetags
		_necessaryGluetags.clear();

#ifndef NDEBUG
		std::cout << "Gluetags: " << _cutEdges.size() << std::endl;
		std::cout << "Edges 'to be bent': " << _mspEdges.size() << std::endl;
#endif

		std::vector<bool> tagged;
		tagged.resize(_facets.size());

		for (gluetab& gt : gluetabs)
		{
			// if the edge the gluetag is attached to is not a cut edge we skip this gluetag
			if (std::find(_cutEdges.begin(), _cutEdges.end(), gt._edge) == _cutEdges.end())
			{
				continue;
			}

			// go through all already added gluetags, if the complimentary gluetag was already added we skip this one
			bool found = false;
			for (gluetab& other : _necessaryGluetags)
			{
				if (gt._edge == other._edge)
				{
					found = true;
					break;
				}
			}

			if (found)
			{
				continue;
			}

			// count all the cut-edge-neighbours of this gluetags edge
			int neighbours = 0;
			for (edge& edge : _cutEdges)
			{
				if (edge.isNeighbour(gt._edge))
				{
					neighbours++;
				}
			}

			// now check how many of the cut-edge-neighbours have a gluetag
			for (gluetab& gluneighbours : _necessaryGluetags)
			{
				if (gt._edge.isNeighbour(gluneighbours._edge))
				{
					neighbours--;
				}
			}

			// if neither the placed Face nor the target Face are tagged OR 2 or more cut-edge-neighbours have no gluetag THEN this one is necessary
			if ((!tagged[size_t(gt._placedFace)] && !tagged[size_t(gt._targetFace)]) || neighbours > 1)
			{
				tagged[size_t(gt._placedFace)] = true;
				tagged[size_t(gt._targetFace)] = true;
				_necessaryGluetags.push_back(gt);
			}
		}

#ifndef NDEBUG
		std::cout << "Necessary Gluetags: " << _necessaryGluetags.size() << std::endl;
#endif
	}

	void graph::unfoldTriangles()
	{
		std::vector<bool> discovered;
		discovered.resize(_facets.size());
		_planarFaces.clear();
		_planarFaces.resize(_facets.size());
		_planarGluetags.clear();
		resetTree();
		unfoldTriangles(0, discovered, 0);
	}

	void graph::unfoldTriangles(int index, std::vector<bool>& discovered, int parent)
	{
		// only the case for the first triangle
		if (index == parent)
		{
			Facet facet = _facets[int(index)];

			_planarFaces[size_t(index)].A = utility::point_to_vector(facet->facet_begin()->vertex()->point());
			_planarFaces[size_t(index)].B = utility::point_to_vector(facet->facet_begin()->next()->vertex()->point());
			_planarFaces[size_t(index)].C = utility::point_to_vector(facet->facet_begin()->next()->next()->vertex()->point());

			utility::planar(_planarFaces[size_t(index)].A, _planarFaces[size_t(index)].B, _planarFaces[size_t(index)].C, _planarFaces[size_t(index)].a, _planarFaces[size_t(index)].b, _planarFaces[size_t(index)].c);

			_planarFaces[size_t(index)].parent = index;
			_planarFaces[size_t(index)].self = index;
		}
		else
		{
			// determine which Vertices are known
			Polyhedron::Halfedge_around_facet_circulator hfc = _facets[int(index)]->facet_begin();
			do
			{
				glm::vec3 Pu = utility::point_to_vector(hfc->vertex()->point());
				// if this vertex is not shared it is the unkown one
				if (Pu != _planarFaces[size_t(parent)].A && Pu != _planarFaces[size_t(parent)].B && Pu != _planarFaces[size_t(parent)].C)
				{ // bottom right
					glm::vec3 P1 = utility::point_to_vector(hfc->next()->vertex()->point());
					glm::vec3 P2 = utility::point_to_vector(hfc->next()->next()->vertex()->point());

					glm::vec2 p1 = _planarFaces[size_t(parent)].get(P1);
					glm::vec2 p2 = _planarFaces[size_t(parent)].get(P2);
					glm::vec2 p3prev = _planarFaces[size_t(parent)].get(_planarFaces[size_t(parent)].get(P1, P2));

					_planarFaces[size_t(index)].A = P1;
					_planarFaces[size_t(index)].B = P2;
					_planarFaces[size_t(index)].C = Pu;
					_planarFaces[size_t(index)].a = p1;
					_planarFaces[size_t(index)].b = p2;

					_planarFaces[size_t(index)].self = int(index);
					_planarFaces[size_t(index)].parent = int(parent);

					utility::planar(P1, P2, Pu, p1, p2, p3prev, _planarFaces[size_t(index)].c);
					break;
				}
			} while (++hfc != _facets[int(index)]->facet_begin());
		}

		discovered[size_t(index)] = true;
		// go through all adjacent edges
		for (size_t i = 0; i < _tree[size_t(index)].size(); ++i)
		{
			if (!discovered[size_t(_tree[size_t(index)][i])])
			{
				unfoldTriangles(int(_tree[int(index)][i]), discovered, int(index));
			}
		}
	}

	void graph::unfoldGluetags()
	{
		_planarGluetags.clear();
		for (gluetab& gluetag : _necessaryGluetags)
		{
			int index = gluetag._placedFace;
			Polyhedron::Halfedge_around_facet_circulator hfc = _facets[index]->facet_begin();
			do
			{
				glm::vec3 Pu = utility::point_to_vector(hfc->vertex()->point());

				// if this vertex is not shared it is the unkown one
				if (Pu != utility::point_to_vector(gluetag._edge._halfedge->vertex()->point())
					&& Pu != utility::point_to_vector(gluetag._edge._halfedge->prev()->vertex()->point()))
				{
					glm::vec3 P1 = utility::point_to_vector(hfc->next()->vertex()->point()); // bottom left
					glm::vec3 P2 = utility::point_to_vector(hfc->next()->next()->vertex()->point()); // bottom right

					glm::vec2 p1 = _planarFaces[size_t(index)].get(P1);
					glm::vec2 p2 = _planarFaces[size_t(index)].get(P2);
					glm::vec2 p3prev = _planarFaces[size_t(index)].get(Pu);

					gluetabToPlane tmp(&gluetag);

					if (P1 == gluetag._bl)
					{
						tmp.a = p1;
						tmp.b = p2;
					}
					else
					{
						tmp.b = p1;
						tmp.a = p2;
					}

					glm::vec2 side = (tmp.b - tmp.a) / 8.0f;

					tmp.b = tmp.b - side;
					tmp.a = tmp.a + side;

					utility::planar(gluetag._bl, gluetag._br, gluetag._tl, tmp.a, tmp.b, p3prev, tmp.c);
					utility::planar(gluetag._bl, gluetag._br, gluetag._tr, tmp.a, tmp.b, p3prev, tmp.d);

					tmp._overlaps = false;
					tmp.faceindex = index;

					_planarGluetags.push_back(tmp);
				}
			} while (++hfc != _facets[int(index)]->facet_begin());
		}
	}

	double graph::findTriangleOverlaps()
	{
		double overlaps = 0;

		for (size_t i = 0; i < _planarFaces.size(); i++)
		{
			if (_planarFaces[i]._overlaps)
				continue;

			for (size_t j = i + 1; j < _planarFaces.size(); j++)
			{
				if (_planarFaces[j]._overlaps)
					continue;

				if (_planarFaces[i].self == _planarFaces[j].self || _planarFaces[i].parent == _planarFaces[j].self
					|| _planarFaces[i].self == _planarFaces[j].parent || _planarFaces[i].parent == _planarFaces[j].parent)
					continue;

				double area = _planarFaces[j].overlaps(_planarFaces[i]);
				if (area > 0)
				{
					overlaps += area * 1000;
					_planarFaces[j]._overlaps = true;
					_planarFaces[i]._overlaps = true;
					break;
				}
			}
		}

		return overlaps;
	}

	double graph::findGluetagOverlaps()
	{
		double overlaps = 0;

		for (gluetabToPlane& gt : _planarGluetags)
		{
			for (faceToPlane& face : _planarFaces)
			{
				if (gt.faceindex == face.self)
					continue;

				double area = gt.overlaps(face);
				if (area > 0)
				{
					overlaps += area;
					gt._overlaps = true;
					break;
				}
			}
		}

		for (size_t i = 0; i < _planarGluetags.size(); i++)
		{
			for (size_t j = i + 1; j < _planarGluetags.size(); j++)
			{
				double area = _planarGluetags[j].overlaps(_planarGluetags[i]);
				if (area > 0)
				{
					overlaps += area;
					_planarGluetags[j]._overlaps = true;
					break;
				}
			}
		}

		return overlaps;
	}

	void graph::randomMove()
	{
		// take a random edge and change it's weight
		size_t random = size_t(rand()) % (_edges.size() + 0 + 1) + 0;
		_edges[random]._weight = (double(std::rand()) / RAND_MAX);
	}

	void graph::resetTree()
	{
		_tree.clear();
		_tree.resize(_facets.size());
		for (int i = 0; i < int(_facets.size()); ++i)
		{
			for (edge& edge : _mspEdges)
			{
				if (edge._sFace != i)
					continue;

				_tree[size_t(i)].push_back(edge._tFace);
				_tree[size_t(edge._tFace)].push_back(edge._sFace);
			}
		}
	}

	bool graph::isSingleComponent(std::vector<std::vector<int>>& adjacenceList)
	{
		// list storing discovered nodes
		std::vector<bool> discovered(_facets.size());

		// check if it is acyclic from the first node
		if (!isAcyclic(adjacenceList, 0, discovered, -1))
		{
			return false;
		}

		// if not all nodes have been discovered, the graph is not connected
		for (size_t i = 0; i < discovered.size(); i++)
		{

			// if node at index i was not discovered the graph is not connected
			if (!discovered[i])
			{
#ifndef NDEBUG
				std::cout << "not connected face: " << i << std::endl;
#endif
				return false;
			}
		}
		return true;
	}

	bool graph::isAcyclic(std::vector<std::vector<int>> const& adjacenceList, int start, std::vector<bool>& discovered, int parent)
	{
		// mark current node as discovered
		discovered[start] = true;

		// loop through every edge from (start -> node(s))
		for (int node : adjacenceList[start])
		{
			// if this node was not discovered
			if (!discovered[node])
			{
				if (!isAcyclic(adjacenceList, int(node), discovered, int(start))) // start dfs from node
					return false;
			}
			// node is discovered but not a parent => back-edge (cycle)
			else if (node != parent)
			{
				return false;
			}
		}

		// graph is acyclic
		return true;
	}

	void graph::initEdgeWeight()
	{
		// init edge weights
		for (edge& edge : _edges)
		{
			edge._weight = (double(std::rand()) / RAND_MAX);
		}
		// init gluetag probabilities
		for (gluetab& gluetag : _gluetags)
		{
			gluetag._probability = (double(std::rand()) / RAND_MAX);
		}
	}

	float graph::compactness()
	{
		std::vector<glm::vec2> points;
		for (faceToPlane& face : _planarFaces)
		{
			points.push_back(face.a);
			points.push_back(face.b);
			points.push_back(face.c);
		}

		auto xExtrema = std::minmax_element(points.begin(), points.end(), [](const glm::vec2& lhs, const glm::vec2& rhs)
		{ return lhs.x < rhs.x; });
		auto yExtrema = std::minmax_element(points.begin(), points.end(), [](const glm::vec2& lhs, const glm::vec2& rhs)
		{ return lhs.y < rhs.y; });

		glm::vec2 ul(xExtrema.first->x, yExtrema.first->y);
		glm::vec2 lr(xExtrema.second->x, yExtrema.second->y);

		return std::abs(lr.x - ul.x) + std::abs(ul.y - lr.y);
	}

	int graph::find(Facet facet)
	{
		std::map<int, Facet>::iterator it = _facets.begin();

		// iterate through faces of the graph and return the index if found
		while (it != _facets.end())
		{
			if (it->second == facet)
			{
				return it->first;
			}
			it++;
		}
		// else return -1
		return -1;
	}

	bool graph::find(edge& edge)
	{
		return std::find(_edges.begin(), _edges.end(), edge) != _edges.end();
	}
}
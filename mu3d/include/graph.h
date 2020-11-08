#pragma once
#include "glm.hpp"
#include "edge.h"
#include "gluetag.h"
#include "facetoplane.h"
#include "gluetagtoplane.h"

#include <string>
#include <map>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>

typedef CGAL::Polyhedron_3<CGAL::Simple_cartesian<double>>::Halfedge_handle Halfedge;
typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Polyhedron::Facet_handle Facet;


class graph {
public:
	graph();
	void load(std::string file);
	bool optimise(float max_its, float opt_its = 0);
	void save(std::string mainmodel, std::string gluetabs);
private:
	Polyhedron _mesh;
	std::map<int, Facet> _facets;
	std::vector<glm::vec3> _vertices;
	std::vector<short> _indices;
	std::vector<edge> _edges;
	std::vector<gluetag> _gluetags;

	/** Calculated each epoch **/
	std::vector<edge> _mspEdges;
	std::vector<edge> _cutEdges;
	std::vector<gluetag> _necessaryGluetags;
	std::vector<std::vector<size_t>> _tree;

	/** Best calculated Solution **/
	std::vector<edge> _C;
	std::vector<gluetag> _Cgt;
	std::vector<faceToPlane> _CplanarFaces;
	std::vector<gluetagToPlane> _CplanarGluetags;
	std::vector<gluetagToPlane> _CplanarMirrorGT;

	std::vector<faceToPlane> _planarFaces;
	std::vector<gluetagToPlane> _planarGluetags;

	double _Cenergy;
	float _optEnergy;
	double _temperature;
	double _maxtemp;
	double _opttemperature;
	bool _optimise;

	void initialise(float max_its, float opt_its = 0);
	void next();
	void compactIt();

	int find(Facet facet);
	bool find(edge& edge);

	void compute_dual();
	void compute_mst();
	void calculateGlueTags(std::vector<gluetag> gluetags);

	void unfoldTriangles();
	void unfoldTriangles(int index, std::vector<bool>& discovered, int parent);
	void unfoldGluetags();

	double findTriangleOverlaps();
	double findGluetagOverlaps();

	void randomMove();
	void resetTree();

	bool isSingleComponent(std::vector<std::vector<int>>& adjacenceList);
	bool isAcyclic(std::vector<std::vector<int>> const& adjacenceList, int start, std::vector<bool>& discovered, int parent);
	void initEdgeWeight();
	float compactness();
};
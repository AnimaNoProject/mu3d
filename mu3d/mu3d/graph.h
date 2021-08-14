#pragma once
#include "glm.hpp"
#include "edge.h"
#include "gluetab.h"
#include "facetoplane.h"
#include "gluetabtoplane.h"

#include <string>
#include <map>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/squared_distance_3.h>
#include <random>

typedef CGAL::Polyhedron_3<CGAL::Simple_cartesian<double>>::Halfedge_handle Halfedge;
typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polyhedron_3<Kernel> Polyhedron;
typedef Polyhedron::Facet_handle Facet;

namespace mu3d
{
	class graph {
	public:
		graph();
		~graph();
		void load(std::string file);
		bool unfold(int max_its, int opt_its = 0);
		void save(std::string mainmodel, std::string gluetabs);
		void save(std::string mainmodel, std::string gluetabs, std::string gluetabsMirror);
		void save(std::string filepath);
	private:
		Polyhedron _mesh;
		std::map<int, Facet> _facets;
		std::vector<edge> _edges;
		std::vector<gluetab> _gluetags;

		/** Calculated each epoch **/
		std::vector<edge> _mspEdges;
		std::vector<edge> _cutEdges;
		std::vector<gluetab> _necessaryGluetabs;
		std::vector<std::vector<size_t>> _tree;

		/** Best calculated Solution **/
		std::vector<edge> _C;
		std::vector<gluetab> _Cgt;
		std::vector<faceToPlane> _CplanarFaces;
		std::vector<gluetabToPlane> _CplanarGluetabs;
		std::vector<gluetabToPlane> _CplanarMirrorGT;

		std::vector<faceToPlane> _planarFaces;
		std::vector<gluetabToPlane> _planarGluetabs;

		double _Cenergy;
		double _optEnergy;
		int _temperature;
		float _maxtemp;
		float _opttemperature;
		bool _optimise;
		std::default_random_engine _generator;
  		std::uniform_real_distribution<double> _distribution;

		void initialise(int max_its, int opt_its = 0);
		void next();
		void next_optimise();

		int find(Facet facet);
		bool find(edge& edge);

		void compute_dual();
		void compute_mst();
		void compute_gluetabs(std::vector<gluetab> gluetabs);
		void compute_gluetab_target(std::vector<gluetabToPlane>& gluetabs);

		void unfold_mesh();
		void unfold_mesh(int index, std::vector<bool>& discovered, int parent);
		void unfold_gluetabs();

		double overlapping_area_mesh();
		double overlapping_area_gluetabs();

		void rand_step();
		void reset_tree();

		bool is_single_component(std::vector<std::vector<int>>& adjacenceList);
		bool is_acyclic(std::vector<std::vector<int>> const& adjacenceList, int start, std::vector<bool>& discovered, int parent);
		void assign_edge_weights();
		double compactness();
	};
}

extern "C" 	__declspec(dllexport) mu3d::graph * __stdcall _graph();

extern "C" 	__declspec(dllexport) void __stdcall _deleteGraph(mu3d::graph * g);

extern "C" 	__declspec(dllexport) void __stdcall _load(mu3d::graph * g, char* file);

extern "C" 	__declspec(dllexport) bool __stdcall _unfold(mu3d::graph * g, int max_its, int opt_its = 0);

extern "C" 	__declspec(dllexport) void __stdcall _save(mu3d::graph * g, char* mainmodel, char* gluetabs);

extern "C" 	__declspec(dllexport) void __stdcall _save(mu3d::graph * g, char* mainmodel, char* gluetabs, char* gluetabsmirror);

extern "C" 	__declspec(dllexport) void __stdcall _save_unified(mu3d::graph * g, char* filepath);
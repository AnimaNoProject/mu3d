#include "graph.h"

Graph::Graph()
{

}

Graph::~Graph()
{

}

void Graph::addFace(Facet facet)
{
    if(_facets.empty())
    {
        // first facet
        _facets.insert(std::make_pair(0, facet));
    }
    else
    {
        // all other facets get the index+1
        _facets.insert(std::make_pair(_facets.rbegin()->first+1 , facet));
    }
}

void Graph::calculateDual()
{
    // loop through all faces
    for(std::map<int, Facet>::iterator it = _facets.begin(); it != _facets.end(); ++it)
    {
        int faceId = getFacetID(it->second);
        // loop through halfedges of all faces and add the dual edges, that were not added yet
        // use distance as weight
        Polyhedron::Halfedge_around_facet_circulator hfc = it->second->facet_begin();
        do
        {
            int oppositeFaceId = getFacetID(hfc->opposite()->facet());
            double distance = sqrt(CGAL::squared_distance(hfc->prev()->vertex()->point(), hfc->vertex()->point()));
            UndirectedEdge edge = UndirectedEdge(faceId, oppositeFaceId);



            //std::cout << "IHE: " << hfc->prev()->vertex()->point() << " to " << hfc->vertex()->point() << std::endl;
            //std::cout << "OHE: " << hfc->opposite()->prev()->vertex()->point() << " to " << hfc->opposite()->vertex()->point()  << std::endl;
            //std::cout << "distance: " << sqrt(CGAL::squared_distance(hfc->prev()->vertex()->point(), hfc->vertex()->point())) << std::endl;
        } while (++hfc != it->second->facet_begin());
        std::cout << std::endl;
    }
}

int Graph::getFacetID(Facet facet)
{
    // loop through all facets
    for(std::map<int, Facet>::iterator it = _facets.begin(); it != _facets.end(); ++it)
    {
        // return matching facets index
        if(it->second == facet)
        {
            return it->first;
        }
    }
    return -1;
}

bool Graph::exists(UndirectedEdge edge)
{

}

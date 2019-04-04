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
    for(std::pair<int, Facet> facet : _facets)
    {
        int faceId = facet.first;

        // loop through halfedges of all faces and add the dual edges, that were not added yet
        // use distance as weight
        Polyhedron::Halfedge_around_facet_circulator hfc = facet.second->facet_begin();
        do
        {
            // get the opposing face
            int oppositeFaceId = getFacetID(hfc->opposite()->facet());

            // use distance as meassurement
            double distance = sqrt(CGAL::squared_distance(hfc->prev()->vertex()->point(), hfc->vertex()->point()));
            Edge edge = Edge(faceId, oppositeFaceId, distance);

            // if this edge doesn't exist already, add it (don't consider direction)
            if(!hasEdge(edge))
            {
                _edges.push_back(edge);
            }

        } while (++hfc != facet.second->facet_begin());
    }
}

void Graph::calculateMSP()
{
    _mspEdges.clear();

    // sort edges from biggest edgelength to smallest, prefer to cut small edges
    std::sort(_edges.begin(), _edges.end());
    std::reverse(_edges.begin(), _edges.end());

    // create the adjacence list
    std::vector<std::vector<int>> adjacenceList;
    adjacenceList.resize(_facets.size());

    // go through all the edges
    for(Edge& edge : _edges)
    {
        // add the edge to the MSP
        _mspEdges.push_back(edge);

        adjacenceList[ulong(edge._sFace)].push_back(edge._tFace);
        adjacenceList[ulong(edge._tFace)].push_back(edge._sFace);

        // list storing discovered nodes
        std::vector<bool> discovered(_facets.size());

        // if the MSP is now cyclic, the added egde needs to be removed again
        for(ulong i = 0; i < _facets.size(); i++)
        {
            // if the node is alone, or already discovered
            if(adjacenceList[i].empty() || discovered[i])
                continue;

            // if the graph is cyclic
            if(!isAcyclic(i, adjacenceList, discovered))
            {
                // remove the edge from the spanning tree
                _mspEdges.erase(remove(_mspEdges.begin(), _mspEdges.end(), edge), _mspEdges.end());
                // add edge to the "to be cut" list
                _cutEdges.push_back(edge);
                // cleanup the adjacence list
                adjacenceList[ulong(edge._sFace)].erase(remove(adjacenceList[ulong(edge._sFace)].begin(), adjacenceList[ulong(edge._sFace)].end(), edge._tFace), adjacenceList[ulong(edge._sFace)].end());
                adjacenceList[ulong(edge._tFace)].erase(remove(adjacenceList[ulong(edge._tFace)].begin(), adjacenceList[ulong(edge._tFace)].end(), edge._sFace), adjacenceList[ulong(edge._tFace)].end());
            }
        }
    }

#ifndef NDEBUG
    std::cout << "number of faces: " << _facets.size() << std::endl;
    std::cout << "number of edges: " << _mspEdges.size() << std::endl;

    // debugging purpose: show all edges of the MSP
    std::cout << "MSP over the edges" << std::endl;
    for(Edge& edge : _mspEdges)
        std::cout << "Edge: " << edge._sFace << "<->" << edge._tFace << std::endl;

    if(isSingleComponent(adjacenceList))
        std::cout << "Graph is a single component!" << std::endl;
    else
        std::cout << "Graph is a NOT single component!" << std::endl;
#endif
}

bool Graph::isSingleComponent(std::vector<std::vector<int>>& adjacenceList)
{
    // list storing discovered nodes
    std::vector<bool> discovered(_facets.size());

    bool isSingleComponent = true;

    // check if it is acyclic from the first node
    isSingleComponent = isAcyclic(0, adjacenceList, discovered);

    for (ulong i = 0; i < discovered.size(); i++)
    {
        if(!discovered[i])
        {
            isSingleComponent = false;
#ifndef NDEBUG
            std::cout << "not connected face: " << i << std::endl;
#endif
        }
    }
    return isSingleComponent;
}

bool Graph::isAcyclic(ulong start, std::vector<std::vector<int>>& adjacenceList, std::vector<bool>& discovered)
{
    return dfs(adjacenceList, start, discovered, -1);
}

bool Graph::dfs(std::vector<std::vector<int>> const &adjacenceList, ulong start, std::vector<bool> &discovered, int parent)
{
    // mark current node as discovered
    discovered[start] = true;

    // loop through every edge from (start -> node(s))
    for(int node : adjacenceList[start])
    {
        // if this node was not discovered
        if (!discovered[ulong(node)])
        {
            if(!dfs(adjacenceList, ulong(node), discovered, int(start))) // start dfs from node
                return false;
        }
        else if (node != parent) // node is discovered but not a parent => back-edge (cycle)
        {
            return false;
        }
    }

    // graph is acyclic
    return true;
}

int Graph::getFacetID(Facet facet)
{
    // loop through all facets
    for(std::pair<int, Facet> pfacet : _facets)
    {
        // if saved facet is the same, return the index
        if(pfacet.second == facet)
        {
            return pfacet.first;
        }
    }

    // if this facet has no match return
    return -1;
}

bool Graph::hasEdge(Edge edge)
{
    // go through all edges and check if the edge(parameter) is equal to one of them
    for(Edge &pedge : _edges)
    {
        if(pedge == edge)
        {
            return true;
        }
    }

    return false;
}

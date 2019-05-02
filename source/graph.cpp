#include "graph.h"

Graph::Graph()
{

}

Graph::~Graph()
{

}

void Graph::unfoldGraph(std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors)
{
    _tree.resize(_facets.size());
    for(int i = 0; i < int(_facets.size()); ++i)
    {
        for(Edge& edge : _mspEdges)
        {
            if(edge._sFace != i)
                continue;

            _tree[ulong(i)].push_back(edge._tFace);
            _tree[ulong(edge._tFace)].push_back(edge._sFace);
        }
    }

    std::vector<bool> discovered;
    discovered.resize(_facets.size());

    std::vector<DimensionMapper> faceMap;
    faceMap.resize(_facets.size());

    treeify(_tree, 0, discovered, 0, faceMap);

    for(DimensionMapper& mapper : faceMap)
    {
        vertices.push_back(QVector3D(mapper.a.x(), mapper.a.y(), 0));
        vertices.push_back(QVector3D(mapper.b.x(), mapper.b.y(), 0));
        vertices.push_back(QVector3D(mapper.c.x(), mapper.c.y(), 0));

        std::cout << mapper.a.x() << ", " << mapper.a.y() << std::endl;
        std::cout << mapper.b.x() << ", " << mapper.b.y() << std::endl;
        std::cout << mapper.c.x() << ", " << mapper.c.y() << std::endl;

        colors.push_back(QVector3D(1,1,1));
        colors.push_back(QVector3D(1,1,1));
        colors.push_back(QVector3D(1,1,1));
    }
}

void Graph::treeify(std::vector<std::vector<int>> const &edges, ulong index, std::vector<bool>& discovered, ulong parent, std::vector<DimensionMapper>& faceMap)
{
    // only the case for the first triangle
    if(index == parent)
    {
        Facet facet = _facets[int(index)];

        faceMap[index].A = Utility::pointToVector(facet->facet_begin()->vertex()->point());
        faceMap[index].B = Utility::pointToVector(facet->facet_begin()->next()->vertex()->point());
        faceMap[index].C = Utility::pointToVector(facet->facet_begin()->next()->next()->vertex()->point());

        float lengthAB = (faceMap[index].A - faceMap[index].B).length();

        faceMap[index].a = QVector2D(0, 0);
        faceMap[index].b = QVector2D(lengthAB, 0);

        float s = QVector3D::crossProduct((faceMap[index].B - faceMap[index].A), (faceMap[index].C - faceMap[index].A)).length() / float(std::pow(lengthAB, 2));
        float c = QVector3D::dotProduct((faceMap[index].B - faceMap[index].A), (faceMap[index].C - faceMap[index].A)) / float(std::pow(lengthAB, 2));

        float c1x = faceMap[index].a.x()
                + c * (faceMap[index].b.x() - faceMap[index].a.x())
                - s * (faceMap[index].b.y() - faceMap[index].a.y());
        float c1y = faceMap[index].a.y()
                + c * (faceMap[index].b.y() - faceMap[index].a.y())
                + s * (faceMap[index].b.x() - faceMap[index].a.x());

        faceMap[index].c = QVector2D(c1x, c1y);
    }
    else
    {
        DimensionMapper* previous = &faceMap[parent];
        DimensionMapper* next = &faceMap[index];
        bool foundA, foundB, foundC;
        foundA = foundB = foundC = false;

        QVector3D unkown3D;
        QVector2D prev2D;

        // determine which Vertices are known
        Polyhedron::Halfedge_around_facet_circulator hfc = _facets[int(index)]->facet_begin();
        do
        {
            QVector3D vector = Utility::pointToVector(hfc->vertex()->point());
            if(vector == previous->A)
            {
                next->A = vector;
                next->a = previous->a;
                foundA = true;
            }
            else if (vector == previous->B)
            {
                next->B = vector;
                next->b = previous->b;
                foundB = true;
            }
            else if (vector == previous->C)
            {
                next->C = vector;
                next->c = previous->c;
                foundC = true;
            }
            else
            {
                unkown3D = vector;
            }
        } while (++hfc != _facets[int(index)]->facet_begin());

        QVector2D p1, p2;
        QVector3D p13D, p23D;

        if(!foundA)
        {
            p1 = next->b;
            p2 = next->c;
            p13D = next->B;
            p23D = next->C;
            prev2D = previous->a;

            next->A = unkown3D;
        }
        else if (!foundB)
        {
            p1 = next->a;
            p2 = next->c;
            p13D = next->A;
            p23D = next->C;
            prev2D = previous->b;
            next->B = unkown3D;
        }
        else if (!foundC)
        {
            p1 = next->a;
            p2 = next->b;
            p13D = next->A;
            p23D = next->B;
            prev2D = previous->c;
            next->C = unkown3D;
        }

        float length = (p1 - p2).length();

        float s = QVector3D::crossProduct((p23D - p13D), (unkown3D - p13D)).length() / float(std::pow(length, 2));
        float unkown = QVector3D::dotProduct((p23D - p13D), (unkown3D - p13D)) / float(std::pow(length, 2));

        float unkown1x = p1.x()
                + unkown * (p2.x() - p1.x())
                - s * (p2.y() - p1.y());
        float unkown1y = p1.y()
                + unkown * (p2.y() - p1.y())
                + s * (p2.x() - p1.x());

        float unkown2x = p1.x()
                + unkown * (p2.x() - p1.x())
                + s * (p2.y() - p1.y());
        float unkown2y = p1.y()
                + unkown * (p2.y() - p1.y())
                - s * (p2.x() - p1.x());

        QVector2D unknownposs(unkown1x, unkown1y);

        float d = (prev2D.x() - p1.x()) * (p2.y() - p1.y()) - (prev2D.y() - p1.y()) * (p2.x() - p1.x());
        float newd = (unknownposs.x() - p1.x()) * (p2.y() - p1.y()) - (unknownposs.y() - p1.y()) * (p2.x() - p1.x());

        // if c1 is on the same side as previous c -> unfold to c2
        if (d < 0 && newd < 0)
        {
            if(!foundA)
                faceMap[index].a = QVector2D(unkown2x, unkown2y);
            else if(!foundB)
                faceMap[index].b = QVector2D(unkown2x, unkown2y);
            else if(!foundC)
                faceMap[index].c = QVector2D(unkown2x, unkown2y);
        }
        else
        {
            if(!foundA)
                faceMap[index].a = QVector2D(unkown1x, unkown1y);
            else if(!foundB)
                faceMap[index].b = QVector2D(unkown1x, unkown1y);
            else if(!foundC)
                faceMap[index].c = QVector2D(unkown1x, unkown1y);
        }
    }

    // go through all adjacent edges
    discovered[index] = true;
    for(ulong i = 0; i < edges[index].size(); ++i)
    {
        if(!discovered[ulong(edges[index][i])])
        {
            treeify(edges, ulong(edges[index][i]), discovered, index, faceMap);
        }
    }
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

            // center of the edge
            QVector3D center = (Utility::pointToVector(hfc->prev()->vertex()->point()) + Utility::pointToVector(hfc->vertex()->point())) / 2;

            Edge edge = Edge(faceId, oppositeFaceId, distance, center, hfc, _facets[faceId], _facets[oppositeFaceId]);

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
    // clear the previous msp edges
    _mspEdges.clear();
    _cutEdges.clear();

    // sort edges from smallest to biggest, better cut big ones
    std::sort(_edges.begin(), _edges.end());

    // create the adjacence list
    std::vector<std::vector<int>> adjacenceList;
    adjacenceList.resize(_facets.size());

    // go through all possible edges
    for(Edge& edge : _edges)
    {
        // add edge to msp, add adjacent faces
        _mspEdges.push_back(edge);
        adjacenceList[ulong(edge._sFace)].push_back(edge._tFace);
        adjacenceList[ulong(edge._tFace)].push_back(edge._sFace);

        // list storing discovered nodes
        std::vector<bool> discovered(_facets.size());

        // if the MSP is now cyclic, the added egde needs to be removed again
        for(ulong i = 0; i < _facets.size(); i++)
        {
            // if the node is alone (no incident edges), or already discovered, no need to check
            if(adjacenceList[i].empty() || discovered[i])
                continue;

            // if the graph is cyclic
            if(!isAcyclic(adjacenceList, i, discovered, -1))
            {
                _mspEdges.erase(remove(_mspEdges.begin(), _mspEdges.end(), edge), _mspEdges.end());

                // add edge to the "to be cut" list
                _cutEdges.push_back(edge);

                // cleanup the adjacence list
                adjacenceList[ulong(edge._sFace)].erase(remove(adjacenceList[ulong(edge._sFace)].begin(), adjacenceList[ulong(edge._sFace)].end(), edge._tFace), adjacenceList[ulong(edge._sFace)].end());
                adjacenceList[ulong(edge._tFace)].erase(remove(adjacenceList[ulong(edge._tFace)].begin(), adjacenceList[ulong(edge._tFace)].end(), edge._sFace), adjacenceList[ulong(edge._tFace)].end());

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
    for(Edge& edge : _mspEdges)
        std::cout << "Edge: " << edge._sFace << "<->" << edge._tFace << std::endl;

    // check if the graph is a single component
    if(isSingleComponent(adjacenceList))
        std::cout << "Graph is a single component!" << std::endl;
    else
        std::cout << "Graph is a NOT single component!" << std::endl;
#endif
}

void Graph::calculateGlueTags(std::vector<QVector3D>& gtVertices, std::vector<GLushort>& gtIndices, std::vector<QVector3D>& gtColors)
{
    // clear old gluetags
    _gluetags.clear();

#ifndef NDEBUG
    std::cout << "Gluetags: " << _cutEdges.size() << std::endl;
    std::cout << "Edges 'to be bent': " << _mspEdges.size() << std::endl;
#endif


    std::vector<bool> tagged;
    bool tag = false;
    tagged.resize(_facets.size());

    // go through all cut edges and add a gluetag for each
    for(Edge& edge : _cutEdges)
    {

        if(tagged[ulong(edge._sFace)] || tagged[ulong(edge._tFace)])
        {
            tag = true;
        }

        Gluetag gt = Gluetag(edge);
        _gluetags.push_back(gt);

        if(!tag)
        {
            _necessaryGluetags.push_back(gt);
            gt.getVertices(gtVertices, gtIndices, gtColors);
            tagged[ulong(gt._placedFace)] = true;
            tagged[ulong(gt._targetFace)] = true;
        }

        tag = false;
    }

#ifndef NDEBUG
    std::cout << "Necessary Gluetags: " << _necessaryGluetags.size() << std::endl;
#endif
}

bool Graph::isSingleComponent(std::vector<std::vector<int>>& adjacenceList)
{
    // list storing discovered nodes
    std::vector<bool> discovered(_facets.size());

    bool isSingleComponent = true;

    // check if it is acyclic from the first node
    isSingleComponent = isAcyclic(adjacenceList, 0, discovered, -1);

    // if not all nodes have been discovered, the graph is not connected
    for (ulong i = 0; i < discovered.size(); i++)
    {

        // if node at index i was not discovered the graph is not connected
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

bool Graph::isAcyclic(std::vector<std::vector<int>> const &adjacenceList, ulong start, std::vector<bool> &discovered, int parent)
{
    // mark current node as discovered
    discovered[start] = true;

    // loop through every edge from (start -> node(s))
    for(int node : adjacenceList[start])
    {
        // if this node was not discovered
        if (!discovered[ulong(node)])
        {
            if(!isAcyclic(adjacenceList, ulong(node), discovered, int(start))) // start dfs from node
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
class Graph;

void Graph::lines(std::vector<QVector3D>& lineVertices, std::vector<QVector3D>& lineColors)
{
    // loop through all edges
    for(Edge& edge : _mspEdges)
    {
        // source face (center)
        lineVertices.push_back(faceCenter(_facets[edge._sFace]));
        // to middle of the edge
        lineVertices.push_back(edge._middle);
        // middle of the edge
        lineVertices.push_back(edge._middle);
        // to target face (center)
        lineVertices.push_back(faceCenter(_facets[edge._tFace]));

        lineColors.push_back(QVector3D(0.2f, 0.8f, 0.2f));
        lineColors.push_back(QVector3D(0.2f, 0.8f, 0.2f));
        lineColors.push_back(QVector3D(0.2f, 0.8f, 0.2f));
        lineColors.push_back(QVector3D(0.2f, 0.8f, 0.2f));
    }

    // add all cut edges
    for(Edge& edge : _cutEdges)
    {     
        lineVertices.push_back(Utility::pointToVector(edge._halfedge->prev()->vertex()->point()));
        lineVertices.push_back(Utility::pointToVector(edge._halfedge->vertex()->point()));

        lineColors.push_back(QVector3D(0.8f, 0.2f, 0.2f));
        lineColors.push_back(QVector3D(0.8f, 0.2f, 0.2f));
    }

    // add all edges that are not cut edges
    for(Edge& edge : _edges)
    {
        if(std::find(_cutEdges.begin(), _cutEdges.end(), edge) == _cutEdges.end())
        {
            lineVertices.push_back(Utility::pointToVector(edge._halfedge->prev()->vertex()->point()));
            lineVertices.push_back(Utility::pointToVector(edge._halfedge->vertex()->point()));

            lineColors.push_back(QVector3D(0.0f, 0.0f, 0.0f));
            lineColors.push_back(QVector3D(0.0f, 0.0f, 0.0f));
        }
    }
}

QVector3D Graph::faceCenter(Facet facet)
{
    QVector3D middle(0,0,0);

    // add all vertices of the face
    Polyhedron::Halfedge_around_facet_circulator hfc = facet->facet_begin();
    do
    {
        middle += Utility::pointToVector(hfc->vertex()->point());
    } while (++hfc != facet->facet_begin());

    // divide by 3 to get the center
    middle /= 3.0f;

    return middle;
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

bool Graph::hasEdge(Edge& edge)
{
    // loop through all edges
    for(Edge& pedge : _edges)
    {
        // return true if this edge was added to the graph
        if(pedge == edge)
        {
            return true;
        }
    }
    return false;
}

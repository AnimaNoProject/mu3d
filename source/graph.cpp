#include "graph.h"

Graph::Graph()
{

}

Graph::~Graph()
{

}

void Graph::nextC()
{
    move();

    calculateMSP();
    calculateGlueTags();
    resetTree();

    std::vector<GluetagToPlane> gtMap;
    std::vector<FaceToPlane> faceMap;
    std::vector<bool> discovered;

    discovered.resize(_facets.size());
    faceMap.resize(_facets.size());

    double overlaps = unfold(_tree, 0, discovered, 0, faceMap, gtMap);
    double newEnergy = overlaps;

    double energyDelta = newEnergy - _Cenergy;

    if(energyDelta < 0)
    {
        _Cgt = _necessaryGluetags;
        _C = _edges;
        _Cenergy = newEnergy;
    }
    else if (std::exp(energyDelta/temperature) > (double(std::rand()) / RAND_MAX))
    {
        _Cgt = _necessaryGluetags;
        _C = _edges;
        _Cenergy = newEnergy;
    }

    _edges = _C;
    _necessaryGluetags = _Cgt;

    calculateMSP();
    calculateGlueTags();

    temperature -= EPOCH;
}

void Graph::initC()
{
    calculateDual();
    calculateMSP();
    calculateGlueTags();

    resetTree();

    temperature = TEMP_MAX;

    _Cgt = _necessaryGluetags;
    _C = _edges;

    std::vector<GluetagToPlane> gtMap;
    std::vector<FaceToPlane> faceMap;
    std::vector<bool> discovered;

    discovered.resize(_facets.size());
    faceMap.resize(_facets.size());

    double overlaps = unfold(_tree, 0, discovered, 0, faceMap, gtMap);
    _Cenergy = overlaps;
}

void Graph::move()
{
    // assign a probability of every edge to be chose
    _edges = _C;
    _necessaryGluetags = _Cgt;

    for(Edge& edge : _edges)
    {
        edge._probability = (double(std::rand()) / RAND_MAX);
    }

    for(Gluetag& gluetag : _gluetags)
    {
        gluetag._probability = (double(std::rand()) / RAND_MAX);
    }
}

bool Graph::over()
{
    return temperature <= TEMP_MIN;
}

double Graph::energy()
{
    return _Cenergy;
}

void Graph::resetTree()
{
    _tree.clear();
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
}

void Graph::oglPlanar(std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colorsLines, QMatrix4x4& center)
{
    std::vector<GluetagToPlane> gtMap;
    std::vector<FaceToPlane> faceMap;
    std::vector<bool> discovered;

    discovered.resize(_facets.size());
    faceMap.resize(_facets.size());

    resetTree();
    unfold(_tree, 0, discovered, 0, faceMap, gtMap);

    QVector2D planarCenter(0,0);
    center.setToIdentity();

    for(FaceToPlane& mapper : faceMap)
    {
        mapper.drawproperties(vertices, verticesLines, colors);
        planarCenter += (mapper.a + mapper.b + mapper.c) / 3 / faceMap.size();
    }

    for(GluetagToPlane& mapper : gtMap)
    {
        mapper.drawproperties(vertices, verticesLines, colors);
    }

    colorsLines.resize(verticesLines.size());
    center.translate(QVector3D(0,0,0) - QVector3D(planarCenter, 0));
}

void Graph::planar(QVector3D const &A, QVector3D const &B, QVector3D const &C, QVector2D& a, QVector2D& b, QVector2D& c)
{
    float lengthAB = (A - B).length();

    a = QVector2D(0, 0);
    b = QVector2D(lengthAB, 0);

    float s = QVector3D::crossProduct(B - A, C - A).length() / float(std::pow(lengthAB, 2));
    float cl = QVector3D::dotProduct(B - A, C - A) / float(std::pow(lengthAB, 2));

    c = QVector2D(a.x() + cl * (b.x() - a.x()) - s * (b.y() - a.y()),
                   a.y() + cl * (b.y() - a.y()) + s * (b.x() - a.x()));
}

void Graph::planar(QVector3D const &P1, QVector3D const &P2, QVector3D const &Pu, QVector2D const &p1, QVector2D const &p2, QVector2D const &p3prev,  QVector2D& pu)
{
    float length = (p1 - p2).length();

    float s = QVector3D::crossProduct((P2 - P1), (Pu - P1)).length() / float(std::pow(length, 2));
    float unkown = QVector3D::dotProduct((P2 - P1), (Pu - P1)) / float(std::pow(length, 2));

    QVector2D pu1 = QVector2D(p1.x() + unkown * (p2.x() - p1.x()) + s * (p2.y() - p1.y()),
                              p1.y() + unkown * (p2.y() - p1.y()) - s * (p2.x() - p1.x()));

    QVector2D pu2 = QVector2D(p1.x() + unkown * (p2.x() - p1.x()) - s * (p2.y() - p1.y()),
                              p1.y() + unkown * (p2.y() - p1.y()) + s * (p2.x() - p1.x()));

    // the points that are not shared by the triangles need to be on opposite sites
    if (((((p3prev.x() - p1.x()) * (p2.y() - p1.y()) - (p3prev.y() - p1.y()) * (p2.x() - p1.x()) < 0)
     && ((pu1.x() - p1.x()) * (p2.y() - p1.y()) - (pu1.y() - p1.y()) * (p2.x() - p1.x()) > 0)))
    ||
    (
    (((p3prev.x() - p1.x()) * (p2.y() - p1.y()) - (p3prev.y() - p1.y()) * (p2.x() - p1.x()) > 0)
         && ((pu1.x() - p1.x()) * (p2.y() - p1.y()) - (pu1.y() - p1.y()) * (p2.x() - p1.x()) < 0))
    ))
    {
        pu = pu1;
    }
    else
    {
        pu = pu2;
    }
}

int Graph::unfold(std::vector<std::vector<int>> const &edges, ulong index, std::vector<bool>& discovered, ulong parent, std::vector<FaceToPlane>& faceMap, std::vector<GluetagToPlane>& gtMap)
{
    int overlaps = 0;

    // only the case for the first triangle
    if(index == parent)
    {
        Facet facet = _facets[int(index)];

        faceMap[index].A = Utility::pointToVector(facet->facet_begin()->vertex()->point());
        faceMap[index].B = Utility::pointToVector(facet->facet_begin()->next()->vertex()->point());
        faceMap[index].C = Utility::pointToVector(facet->facet_begin()->next()->next()->vertex()->point());

        planar(faceMap[index].A, faceMap[index].B, faceMap[index].C, faceMap[index].a, faceMap[index].b, faceMap[index].c);
    }
    else
    {  
        // determine which Vertices are known
        Polyhedron::Halfedge_around_facet_circulator hfc = _facets[int(index)]->facet_begin();
        do
        {
            QVector3D Pu = Utility::pointToVector(hfc->vertex()->point());
            // if this vertex is not shared it is the unkown one
            if(Pu != faceMap[parent].A && Pu != faceMap[parent].B && Pu != faceMap[parent].C)
            { // bottom right
                QVector3D P1 = Utility::pointToVector(hfc->next()->vertex()->point());
                QVector3D P2 = Utility::pointToVector(hfc->next()->next()->vertex()->point());

                QVector2D p1 = faceMap[parent].get(P1);
                QVector2D p2 = faceMap[parent].get(P2);
                QVector2D p3prev = faceMap[parent].get(faceMap[parent].get(P1, P2));

                faceMap[index].A = P1;
                faceMap[index].B = P2;
                faceMap[index].C = Pu;
                faceMap[index].a = p1;
                faceMap[index].b = p2;

                planar(P1, P2, Pu, p1, p2, p3prev, faceMap[index].c);
                break;
            }
        } while (++hfc != _facets[int(index)]->facet_begin());
    }

    faceMap[index].faceId = index;

    for(Gluetag& gluetag : _necessaryGluetags)
    {
        if(gluetag._placedFace == int(index))
        {
            Polyhedron::Halfedge_around_facet_circulator hfc = _facets[int(index)]->facet_begin();
            do
            {
                QVector3D Pu = Utility::pointToVector(hfc->vertex()->point());

                // if this vertex is not shared it is the unkown one
                if(Pu != Utility::pointToVector(gluetag._edge._halfedge->vertex()->point())
                && Pu != Utility::pointToVector(gluetag._edge._halfedge->prev()->vertex()->point()))
                {
                    QVector3D P1 = Utility::pointToVector(hfc->next()->vertex()->point()); // bottom left
                    QVector3D P2 = Utility::pointToVector(hfc->next()->next()->vertex()->point()); // bottom right

                    QVector2D p1 = faceMap[index].get(P1);
                    QVector2D p2 = faceMap[index].get(P2);
                    QVector2D p3prev = faceMap[index].get(Pu);

                    GluetagToPlane tmp(gluetag);

                    // top edge of the gluetag is 1/4 of the size of the base edge
                    QVector2D side = (p2 - p1) / 4;

                    // adjust size of base of gluetag
                    if(P1 == gluetag._bl)
                    {
                        tmp.a = p1 + side * 0.5;
                        tmp.b = p2 - side * 0.5;
                    }
                    else
                    {
                        tmp.b = p1 + side * 0.5;
                        tmp.a = p2 - side * 0.5;
                    }

                    planar(gluetag._bl, gluetag._br, gluetag._tl, tmp.a, tmp.b, p3prev, tmp.c);

                    planar(gluetag._bl, gluetag._tl, gluetag._tr, tmp.a, tmp.c, tmp.a, tmp.d);

                    tmp.overlapping = false;
                    // check if any overlaps occured with other faces

                    for(ulong i = 0; i < discovered.size(); i++)
                    {
                        if(tmp._gluetag._placedFace == int(i))
                        {
                            continue;
                        }

                        if(tmp.overlaps(faceMap[i]))
                        {
                            tmp.overlapping = true;
                            overlaps++;
                        }
                    }

                    // or overlaps with any existing gluetags
                    for(GluetagToPlane& gtp : gtMap)
                    {
                        if(gtp.overlaps(tmp))
                        {
                            tmp.overlapping = true;
                            overlaps++;
                        }
                    }

                    gtMap.push_back(tmp);
                }
            } while (++hfc != _facets[int(index)]->facet_begin());
        }
    }

    faceMap[index].color = QVector3D(1,1,1);

    // check if any overlaps occured with other faces
    for(ulong i = 0; i < discovered.size(); i++)
    {
        if(!discovered[i] || i == index || i == parent)
        {
            continue;
        }

        if(faceMap[index].overlaps(faceMap[i]))
        {
            faceMap[index].color = QVector3D(1,0,0);
            overlaps++;
        }
    }

    // or overlaps with any existing gluetags
    for(GluetagToPlane& gtp : gtMap)
    {
        if(gtp._gluetag._placedFace == int(index))
        {
            continue;
        }

        if(gtp.overlaps(faceMap[index]))
        {
            faceMap[index].color = QVector3D(1,0,0);
            overlaps++;
        }
    }

    discovered[index] = true;
    // go through all adjacent edges
    for(ulong i = 0; i < edges[index].size(); ++i)
    {
        if(!discovered[ulong(edges[index][i])])
        {
            overlaps += unfold(edges, ulong(edges[index][i]), discovered, index, faceMap, gtMap);
        }
    }

    return overlaps;
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

    // calculate all possible gluetags
    for (Edge& edge : _edges)
    {
        Gluetag gt = Gluetag(edge, true);
        _gluetags.push_back(gt);
        gt = Gluetag(edge, false);
        _gluetags.push_back(gt);
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

void Graph::oglGluetags(std::vector<QVector3D>& gtVertices, std::vector<GLushort>& gtIndices, std::vector<QVector3D>& gtColors)
{
    for(Gluetag& gluetag : _Cgt)
    {
        gluetag.getVertices(gtVertices, gtIndices, gtColors);
    }
}

void Graph::calculateGlueTags()
{
    // clear old gluetags
    _necessaryGluetags.clear();

    std::sort(_gluetags.begin(), _gluetags.end());

#ifndef NDEBUG
    std::cout << "Gluetags: " << _cutEdges.size() << std::endl;
    std::cout << "Edges 'to be bent': " << _mspEdges.size() << std::endl;
#endif

    std::vector<bool> tagged;
    tagged.resize(_facets.size());

    for(Gluetag& gluetag : _gluetags)
    {
        int neighbours = 0;
        bool found = false;

        for(Edge& pedge : _cutEdges)
        {
            // return true if this edge was added to the graph
            if(pedge == gluetag._edge)
            {
                found = true;
                break;
            }
        }

        if(!found)
        {
            continue;
        }

        found = false;

        for(Gluetag& other : _necessaryGluetags)
        {
            if(gluetag._edge == other._edge)
            {
                found = true;
                break;
            }
        }

        if(found)
        {
            continue;
        }

        for(Edge& edge : _cutEdges)
        {
            if(gluetag._edge._halfedge->vertex() == edge._halfedge->vertex()
              || gluetag._edge._halfedge->prev()->vertex() == edge._halfedge->vertex()
              || gluetag._edge._halfedge->vertex() == edge._halfedge->prev()->vertex()
              || gluetag._edge._halfedge->prev()->vertex() == edge._halfedge->prev()->vertex())
            {
                neighbours++;
            }
        }

        for(Gluetag& gluneighbours : _necessaryGluetags)
        {
            if(gluetag._edge._halfedge->vertex() == gluneighbours._edge._halfedge->vertex()
              || gluetag._edge._halfedge->prev()->vertex() == gluneighbours._edge._halfedge->vertex()
              || gluetag._edge._halfedge->vertex() == gluneighbours._edge._halfedge->prev()->vertex()
              || gluetag._edge._halfedge->prev()->vertex() == gluneighbours._edge._halfedge->prev()->vertex())
            {
                neighbours--;
            }
        }

        if((!tagged[ulong(gluetag._placedFace)] && !tagged[ulong(gluetag._targetFace)]) || neighbours > 1)
        {
            tagged[ulong(gluetag._placedFace)] = true;
            tagged[ulong(gluetag._targetFace)] = true;
            _necessaryGluetags.push_back(gluetag);
        }
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

    // check if it is acyclic from the first nodeedges
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

void Graph::oglLines(std::vector<QVector3D>& lineVertices, std::vector<QVector3D>& lineColors)
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

        lineColors.push_back(QVector3D(1.0f, 0.0f, 0.0f));
        lineColors.push_back(QVector3D(1.0f, 0.0f, 0.0f));
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

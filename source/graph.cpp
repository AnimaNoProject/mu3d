#include "graph.h"

Graph::Graph()
{
}

Graph::~Graph()
{

}

bool Graph::neighbourState()
{
    bool redraw = false;

    randomMove();

    // calculate a new spanning tree and gluetags
    calculateMSP();
    calculateGlueTags();

    // unfold and check for overlaps
    std::pair<int, int> overlaps = unfold();
    double newEnergy = overlaps.first;

    double chance = std::pow(std::exp(1), -((TEMP_MAX - temperature)/(TEMP_MAX/50))) / 50;
    double random = (double(std::rand()) / RAND_MAX);

    // if it got better we take the new graph
    if(newEnergy < _Cenergy)
    {
        _Cgt = _gluetags;
        _C = _edges;
        _Cenergy = newEnergy;

        _CplanarFaces.clear();
        _CplanarGluetags.clear();

        _CplanarFaces = _planarFaces;
        _CplanarGluetags = _planarGluetags;

        resetCounter = 0;
        redraw = true;
    }
    // if it is worse, there is a chance we take the worse one (helps getting out of local minimum
    else if (chance > random || resetCounter > RESET_WAIT)
    {
        _Cgt = _gluetags;
        _C = _edges;
        _Cenergy = newEnergy;

        _CplanarFaces.clear();
        _CplanarGluetags.clear();

        _CplanarFaces = _planarFaces;
        _CplanarGluetags = _planarGluetags;

        resetCounter = 0;
        redraw = true;
    }
    else
    {
        resetCounter++;
    }

    // continue working with the best
    _edges = _C;
    _gluetags = _Cgt;

    // end epoch
    temperature -= EPOCH;

    return redraw;
}

void Graph::initializeState()
{
    resetCounter = 0;

    // init edge weights
    for(Edge& edge : _edges)
    {
        edge._weight = (double(std::rand()) / RAND_MAX);
    }
    // init gluetag probabilities
    for(Gluetag& gluetag : _gluetags)
    {
        gluetag._probability = 1 - (double(std::rand()) / RAND_MAX);
    }

    // calculate the dualgraph and an initial MSP and Gluetags
    calculateDual();
    calculateMSP();
    calculateGlueTags();

    temperature = TEMP_MAX;

    // initialize the energy with this unfolding
    std::pair<int, int> overlaps = unfold();

    // it is the best we have
    _Cgt = _gluetags;
    _C = _edges;
    _Cenergy = overlaps.first;
    _CplanarFaces = _planarFaces;
    _CplanarGluetags = _planarGluetags;
}

void Graph::randomMove()
{
    while(std::is_sorted(_edges.begin(), _edges.end()))
    {
        // take a random edge and change it's weight
        ulong random = ulong(rand())%(_edges.size() + 0 + 1) + 0;
        _edges[random]._weight = (double(std::rand()) / RAND_MAX);
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
    QVector2D planarCenter(0,0);
    center.setToIdentity();

    for(FaceToPlane& mapper : _CplanarFaces)
    {
        mapper.drawproperties(vertices, verticesLines, colors);
        planarCenter += (mapper.a + mapper.b + mapper.c) / 3 / _planarFaces.size();
    }

    for(GluetagToPlane& mapper : _CplanarGluetags)
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

std::pair<int, int> Graph::unfold()
{
    std::vector<bool> discovered;
    discovered.resize(_facets.size());
    _planarFaces.clear();
    _planarFaces.resize(_facets.size());
    _planarGluetags.clear();
    resetTree();

    return unfold(0, discovered, 0);
}

std::pair<int, int> Graph::unfold(ulong index, std::vector<bool>& discovered, ulong parent)
{
    std::pair<int, int> overlaps;
    overlaps.first = 0;
    overlaps.second = 0;

    // only the case for the first triangle
    if(index == parent)
    {
        Facet facet = _facets[int(index)];

        _planarFaces[index].A = Utility::pointToVector(facet->facet_begin()->vertex()->point());
        _planarFaces[index].B = Utility::pointToVector(facet->facet_begin()->next()->vertex()->point());
        _planarFaces[index].C = Utility::pointToVector(facet->facet_begin()->next()->next()->vertex()->point());

        planar(_planarFaces[index].A, _planarFaces[index].B, _planarFaces[index].C, _planarFaces[index].a, _planarFaces[index].b, _planarFaces[index].c);
    }
    else
    {  
        // determine which Vertices are known
        Polyhedron::Halfedge_around_facet_circulator hfc = _facets[int(index)]->facet_begin();
        do
        {
            QVector3D Pu = Utility::pointToVector(hfc->vertex()->point());
            // if this vertex is not shared it is the unkown one
            if(Pu != _planarFaces[parent].A && Pu != _planarFaces[parent].B && Pu != _planarFaces[parent].C)
            { // bottom right
                QVector3D P1 = Utility::pointToVector(hfc->next()->vertex()->point());
                QVector3D P2 = Utility::pointToVector(hfc->next()->next()->vertex()->point());

                QVector2D p1 = _planarFaces[parent].get(P1);
                QVector2D p2 = _planarFaces[parent].get(P2);
                QVector2D p3prev = _planarFaces[parent].get(_planarFaces[parent].get(P1, P2));

                _planarFaces[index].A = P1;
                _planarFaces[index].B = P2;
                _planarFaces[index].C = Pu;
                _planarFaces[index].a = p1;
                _planarFaces[index].b = p2;

                planar(P1, P2, Pu, p1, p2, p3prev, _planarFaces[index].c);
                break;
            }
        } while (++hfc != _facets[int(index)]->facet_begin());
    }

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

                    QVector2D p1 = _planarFaces[index].get(P1);
                    QVector2D p2 = _planarFaces[index].get(P2);
                    QVector2D p3prev = _planarFaces[index].get(Pu);

                    GluetagToPlane tmp(&gluetag);

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
                        if(tmp._gluetag->_placedFace == int(i))
                        {
                            continue;
                        }

                        if(tmp.overlaps(_planarFaces[i]))
                        {
                            tmp.overlapping = true;
                            overlaps.second++;
                            break;
                        }
                    }

                    // or overlaps with any existing gluetags
                    for(GluetagToPlane& gtp : _planarGluetags)
                    {
                        if(gtp.overlaps(tmp))
                        {
                            tmp.overlapping = true;
                            overlaps.second++;
                            break;
                        }
                    }

                    _planarGluetags.push_back(tmp);
                }
            } while (++hfc != _facets[int(index)]->facet_begin());
        }
    }

    // check if any overlaps occured with other faces
    for(ulong i = 0; i < discovered.size(); i++)
    {
        // if the other face was not yet discovered, or it's the curent face or it's the parent
        // it can not overlap anyways
        if(!discovered[i] || i == index || i == parent)
        {
            continue;
        }

        if(_planarFaces[index].overlaps(_planarFaces[i]))
        {
            _planarFaces[index].color = QVector3D(1,0,0);
            overlaps.first++;
            break;
        }
    }

    // or overlaps with any existing gluetags
    for(GluetagToPlane& gtp : _planarGluetags)
    {
        // if it's not the gluetag of the current face and overlaps this face
        if(gtp._gluetag->_placedFace != int(index) && gtp.overlaps(_planarFaces[index]))
        {
            gtp.overlapping = true;
            overlaps.second++;
            break;
        }
    }

    discovered[index] = true;
    // go through all adjacent edges
    for(ulong i = 0; i < _tree[index].size(); ++i)
    {
        if(!discovered[ulong(_tree[index][i])])
        {
            std::pair<int, int> cover = unfold(ulong(_tree[index][i]), discovered, index);
            overlaps.first += cover.first;
            overlaps.second += cover.second;
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
            int oppositeFaceId = find(hfc->opposite()->facet());

            // use distance as meassurement
            double distance = sqrt(CGAL::squared_distance(hfc->prev()->vertex()->point(), hfc->vertex()->point()));

            // center of the edge
            QVector3D center = (Utility::pointToVector(hfc->prev()->vertex()->point()) + Utility::pointToVector(hfc->vertex()->point())) / 2;

            Edge edge = Edge(faceId, oppositeFaceId, distance, center, hfc, _facets[faceId], _facets[oppositeFaceId]);

            // if this edge doesn't exist already, add it (don't consider direction)
            if(!find(edge))
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
    gtVertices.clear();
    gtIndices.clear();
    gtColors.clear();

    for(Gluetag& gluetag : _necessaryGluetags)
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
        // if the edge the gluetag is attached to is not a cut edge we skip this gluetag
        if(std::find(_cutEdges.begin(), _cutEdges.end(), gluetag._edge) == _cutEdges.end())
        {
            continue;
        }

        // go through all already added gluetags, if the complimentary gluetag was already added we skip this one
        bool found = false;
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

        // count all the cut-edge-neighbours of this gluetags edge
        int neighbours = 0;
        for(Edge& edge : _cutEdges)
        {
            if(edge.isNeighbour(gluetag._edge))
            {
                neighbours++;
            }
        }

        // now check how many of the cut-edge-neighbours have a gluetag
        for(Gluetag& gluneighbours : _necessaryGluetags)
        {
            if(gluetag._edge.isNeighbour(gluneighbours._edge))
            {
                neighbours--;
            }
        }

        // if neither the placed Face nor the target Face are tagged OR 2 or more cut-edge-neighbours have no gluetag THEN this one is necessary
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

    // go through all vertices and calculate the middle of the face
    Polyhedron::Halfedge_around_facet_circulator hfc = facet->facet_begin();
    do
    {
        middle += Utility::pointToVector(hfc->vertex()->point()) / 3;
    } while (++hfc != facet->facet_begin());

    return middle;
}

int Graph::find(Facet facet)
{
    std::map<int, Facet>::iterator it = _facets.begin();

    // iterate through faces of the graph and return the index if found
    while(it != _facets.end())
    {
        if(it->second == facet)
        {
            return it->first;
        }
        it++;
    }
    // else return -1
    return -1;
}

bool Graph::find(Edge& edge)
{
    // check if this edge already exists in the graph
    return std::find(_edges.begin(), _edges.end(), edge) != _edges.end();
}

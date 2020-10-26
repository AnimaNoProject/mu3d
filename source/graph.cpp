#include "graph.h"

Graph::Graph()
{
    _optimise = false;
}

Graph::~Graph()
{
}

int Graph::initBruteForce()
{
    calculateDual();

    n = int(_edges.size());
    r = int(_facets.size() - 1);

    v = std::vector<bool>(ulong(n));
    std::fill(v.begin(), v.begin() + r, true);

    _Cenergy = 1000000000;
    _optEnergy = 1000000000;

    return 10000000;
}

int Graph::factorial(int n)
{
    return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

float Graph::compactness()
{
    std::vector<QVector2D> points;
    for(FaceToPlane& face : _planarFaces)
    {
       points.push_back(face.a);
       points.push_back(face.b);
       points.push_back(face.c);
    }

    auto xExtrema = std::minmax_element(points.begin(), points.end(), [](const QVector2D& lhs, const QVector2D& rhs)
    { return lhs.x() < rhs.x();});
    auto yExtrema = std::minmax_element(points.begin(), points.end(), [](const QVector2D& lhs, const QVector2D& rhs)
    { return lhs.y() < rhs.y();});

    QVector2D ul(xExtrema.first->x(), yExtrema.first->y());
    QVector2D lr(xExtrema.second->x(), yExtrema.second->y());

    return std::abs(lr.x() - ul.x()) + std::abs(ul.y() - lr.y());
}

bool Graph::nextBruteForce()
{
    if(std::prev_permutation(v.begin(), v.end()))
    {
        std::vector<int> edges;

        for(int i = 0; i < n; i++)
        {
            if(v[ulong(i)])
            {
                edges.push_back(i);
            }
        }
        if(!calculateMSP(edges))
        {
            return false;
        }
    }
    else
    {
        _Cenergy = 0;
        return false;
    }

    // unfold and check for overlaps
    unfoldTriangles();
    double trioverlaps = findTriangleOverlaps();
    double gtoverlaps = 0;

    if(trioverlaps <= 0)
    {
        std::vector<Gluetag> possibleGluetags;

        for(Gluetag& gluetag : _gluetags)
        {
            for(Edge& edge : _cutEdges)
            {
                if(edge._sFace == gluetag._placedFace || edge._tFace == gluetag._placedFace)
                {
                    possibleGluetags.push_back(gluetag);
                    break;
                }
            }
        }

        do
        {
            calculateGlueTags(possibleGluetags);
            unfoldGluetags();
            gtoverlaps = findGluetagOverlaps();
            if(gtoverlaps <= 0)
            {
                break;
            }
        } while (std::next_permutation(_gluetags.begin(), _gluetags.end()));
    }

    double newEnergy = gtoverlaps + trioverlaps;

    // if it got better we take the new graph
    if(newEnergy <= _Cenergy)
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

    return true;
}

bool Graph::neighbourState()
{
    bool redraw = false;

    randomMove();

    // calculate a new spanning tree and gluetags
    calculateMSP();
    calculateGlueTags(_gluetags);

    // unfold and check for overlaps
    unfoldTriangles();
    double trioverlaps = findTriangleOverlaps();
    double gtoverlaps = 0;

    if(trioverlaps <= 0)
    {
        unfoldGluetags();
        gtoverlaps = findGluetagOverlaps();
    }
    else {
        gtoverlaps = 40;
    }

    double newEnergy = trioverlaps + gtoverlaps;

    double chance = (1 - std::pow(std::exp(1), -(temperature)/TEMP_MAX)) / 2000 / 10;
    double random = (double(std::rand()) / RAND_MAX);

    // if it got better we take the new graph
    if(newEnergy <= _Cenergy)
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

        redraw = true;
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

        redraw = true;
    }
    // continue working with the best
    else
    {
        _edges.clear();
        _gluetags.clear();

        _edges = _C;
        _gluetags = _Cgt;
    }

    if(_Cenergy <= 0)
    {
        _optimise = true;
        _optEnergy = compactness();
    }

    // end epoch
    temperature -= EPOCH;
    return redraw;
}

void Graph::initializeState()
{
    initEdgeWeight();

    // calculate the dualgraph and an initial MSP and Gluetags
    calculateDual();
    calculateMSP();
    calculateGlueTags(_gluetags);

    temperature = TEMP_MAX;
    opttemperature = TEMP_OPT;
    _optimise = false;

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

void Graph::randomMove()
{
    // take a random edge and change it's weight
    ulong random = ulong(rand())%(_edges.size() + 0 + 1) + 0;
    _edges[random]._weight = (double(std::rand()) / RAND_MAX);
}

bool Graph::over()
{
    return temperature <= TEMP_MIN;
}

bool Graph::finishedBruteFroce()
{
    return _Cenergy <= 0;
}

bool Graph::optimise()
{
    bool redraw = false;

    randomMove();

    // calculate a new spanning tree and gluetags
    calculateMSP();
    calculateGlueTags(_gluetags);

    // unfold and check for overlaps
    unfoldTriangles();
    double trioverlaps = findTriangleOverlaps();
    double gtoverlaps = 0;

    if(trioverlaps <= 0)
    {
        unfoldGluetags();
        gtoverlaps = findGluetagOverlaps();

        if(gtoverlaps > 0)
        {
            _edges.clear();
            _gluetags.clear();

            _edges = _C;
            _gluetags = _Cgt;
            return false;
        }
    }
    else {
        _edges.clear();
        _gluetags.clear();

        _edges = _C;
        _gluetags = _Cgt;
        return false;
    }

    float newEnergy = compactness();
    // if it got better we take the new graph
    if(newEnergy <= _optEnergy)
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

        redraw = true;
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
    opttemperature -= EPOCH;
    return redraw;
}

bool Graph::finishedOptimise()
{
    if(opttemperature <= TEMP_MIN)
    {
        return true;
    }
    else {
        return false;
    }
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

void Graph::postProcessPlanar(std::vector<QVector3D>& vertices, std::vector<QVector3D>& colors, std::vector<QVector3D>& verticesLines, std::vector<QVector3D>& colorsLines, QMatrix4x4& center)
{
    _CplanarMirrorGT.clear();
    for(GluetagToPlane& mapper : _CplanarGluetags)
    {
        Polyhedron::Halfedge_around_facet_circulator hfc = _facets[mapper._gluetag->_targetFace]->facet_begin();
        do
        {
            QVector3D Pu = Utility::pointToVector(hfc->vertex()->point());

            // if this vertex is not shared it is the unkown one
            if(Pu != Utility::pointToVector(mapper._gluetag->_edge._halfedge->vertex()->point())
                && Pu != Utility::pointToVector(mapper._gluetag->_edge._halfedge->prev()->vertex()->point()))
            {
                QVector3D P1 = Utility::pointToVector(hfc->next()->vertex()->point()); // bottom left
                QVector3D P2 = Utility::pointToVector(hfc->next()->next()->vertex()->point()); // bottom right

                QVector2D p1 = _CplanarFaces[ulong(mapper._gluetag->_targetFace)].get(P1);
                QVector2D p2 =  _CplanarFaces[ulong(mapper._gluetag->_targetFace)].get(P2);
                QVector2D p3prev =  _CplanarFaces[ulong(mapper._gluetag->_targetFace)].get(Pu);

                GluetagToPlane tmp(mapper._gluetag);

                if(P1 == mapper._gluetag->_bl)
                {
                    tmp.a = p2;
                    tmp.b = p1;
                }
                else
                {
                    tmp.b = p2;
                    tmp.a = p1;
                }

                QVector2D side = (tmp.b - tmp.a) / 8;

                tmp.b = tmp.b - side;
                tmp.a = tmp.a + side;

                Utility::gtMirror(mapper._gluetag->_bl, mapper._gluetag->_br, mapper._gluetag->_tl, tmp.a, tmp.b, p3prev, tmp.c);
                Utility::gtMirror(mapper._gluetag->_bl, mapper._gluetag->_br, mapper._gluetag->_tr, tmp.a, tmp.b, p3prev, tmp.d);

                tmp._overlaps = mapper._overlaps;
                tmp._gluetag = mapper._gluetag;
                tmp.drawproperties(vertices, verticesLines, colors);
                _CplanarMirrorGT.push_back(tmp);
            }
        } while (++hfc != _facets[mapper._gluetag->_targetFace]->facet_begin());
    }

    QVector2D planarCenter(0,0);
    center.setToIdentity();

    for(FaceToPlane& mapper : _CplanarFaces)
    {
        mapper.finalproperties(vertices, verticesLines, colors, _mspEdges);
        planarCenter += (mapper.a + mapper.b + mapper.c) / 3 / _planarFaces.size();
    }

    for(GluetagToPlane& mapper : _CplanarGluetags)
    {
        mapper.drawproperties(vertices, verticesLines, colors);
    }

    colorsLines.resize(verticesLines.size());
    center.translate(QVector3D(0,0,0) - QVector3D(planarCenter, 0));
}

void Graph::unfoldTriangles()
{
    std::vector<bool> discovered;
    discovered.resize(_facets.size());
    _planarFaces.clear();
    _planarFaces.resize(_facets.size());
    _planarGluetags.clear();
    resetTree();
    unfoldTriangles(0, discovered, 0);
}

void Graph::unfoldTriangles(int index, std::vector<bool>& discovered, int parent)
{
    // only the case for the first triangle
    if(index == parent)
    {
        Facet facet = _facets[int(index)];

        _planarFaces[ulong(index)].A = Utility::pointToVector(facet->facet_begin()->vertex()->point());
        _planarFaces[ulong(index)].B = Utility::pointToVector(facet->facet_begin()->next()->vertex()->point());
        _planarFaces[ulong(index)].C = Utility::pointToVector(facet->facet_begin()->next()->next()->vertex()->point());

        Utility::planar(_planarFaces[ulong(index)].A, _planarFaces[ulong(index)].B, _planarFaces[ulong(index)].C, _planarFaces[ulong(index)].a, _planarFaces[ulong(index)].b, _planarFaces[ulong(index)].c);

        _planarFaces[ulong(index)].parent = index;
        _planarFaces[ulong(index)].self = index;
    }
    else
    {
        // determine which Vertices are known
        Polyhedron::Halfedge_around_facet_circulator hfc = _facets[int(index)]->facet_begin();
        do
        {
            QVector3D Pu = Utility::pointToVector(hfc->vertex()->point());
            // if this vertex is not shared it is the unkown one
            if(Pu != _planarFaces[ulong(parent)].A && Pu != _planarFaces[ulong(parent)].B && Pu != _planarFaces[ulong(parent)].C)
            { // bottom right
                QVector3D P1 = Utility::pointToVector(hfc->next()->vertex()->point());
                QVector3D P2 = Utility::pointToVector(hfc->next()->next()->vertex()->point());

                QVector2D p1 = _planarFaces[ulong(parent)].get(P1);
                QVector2D p2 = _planarFaces[ulong(parent)].get(P2);
                QVector2D p3prev = _planarFaces[ulong(parent)].get(_planarFaces[ulong(parent)].get(P1, P2));

                _planarFaces[ulong(index)].A = P1;
                _planarFaces[ulong(index)].B = P2;
                _planarFaces[ulong(index)].C = Pu;
                _planarFaces[ulong(index)].a = p1;
                _planarFaces[ulong(index)].b = p2;

                _planarFaces[ulong(index)].self = int(index);
                _planarFaces[ulong(index)].parent = int(parent);

                Utility::planar(P1, P2, Pu, p1, p2, p3prev, _planarFaces[ulong(index)].c);
                break;
            }
        } while (++hfc != _facets[int(index)]->facet_begin());
    }

    discovered[ulong(index)] = true;
    // go through all adjacent edges
    for(ulong i = 0; i < _tree[ulong(index)].size(); ++i)
    {
        if(!discovered[ulong(_tree[ulong(index)][i])])
        {
            unfoldTriangles(_tree[ulong(index)][i], discovered, index);
        }
    }
}

void Graph::unfoldGluetags()
{
    _planarGluetags.clear();
    for(Gluetag& gluetag : _necessaryGluetags)
    {
        int index = gluetag._placedFace;
        Polyhedron::Halfedge_around_facet_circulator hfc = _facets[index]->facet_begin();
        do
        {
            QVector3D Pu = Utility::pointToVector(hfc->vertex()->point());

            // if this vertex is not shared it is the unkown one
            if(Pu != Utility::pointToVector(gluetag._edge._halfedge->vertex()->point())
                && Pu != Utility::pointToVector(gluetag._edge._halfedge->prev()->vertex()->point()))
            {
                QVector3D P1 = Utility::pointToVector(hfc->next()->vertex()->point()); // bottom left
                QVector3D P2 = Utility::pointToVector(hfc->next()->next()->vertex()->point()); // bottom right

                QVector2D p1 = _planarFaces[ulong(index)].get(P1);
                QVector2D p2 = _planarFaces[ulong(index)].get(P2);
                QVector2D p3prev = _planarFaces[ulong(index)].get(Pu);

                GluetagToPlane tmp(&gluetag);

                if(P1 == gluetag._bl)
                {
                    tmp.a = p1;
                    tmp.b = p2;
                }
                else
                {
                    tmp.b = p1;
                    tmp.a = p2;
                }

                QVector2D side = (tmp.b - tmp.a) / 8;

                tmp.b = tmp.b - side;
                tmp.a = tmp.a + side;

                Utility::planar(gluetag._bl, gluetag._br, gluetag._tl, tmp.a, tmp.b, p3prev, tmp.c);
                Utility::planar(gluetag._bl, gluetag._br, gluetag._tr, tmp.a, tmp.b, p3prev, tmp.d);

                tmp._overlaps = false;
                tmp.faceindex = index;

                _planarGluetags.push_back(tmp);
            }
        } while (++hfc != _facets[int(index)]->facet_begin());
    }
}

double Graph::findGluetagOverlaps()
{
    double overlaps = 0;

    for(GluetagToPlane& gt : _planarGluetags)
    {
        for(FaceToPlane& face : _planarFaces)
        {
            if(gt.faceindex == face.self)
                continue;

            double area = gt.overlaps(face);
            if(area > 0)
            {
                overlaps += area;
                gt._overlaps = true;
                break;
            }
        }
    }

    for(size_t i = 0; i < _planarGluetags.size(); i++)
    {
        for(size_t j = i + 1; j < _planarGluetags.size(); j++)
        {
            double area = _planarGluetags[j].overlaps(_planarGluetags[i]);
            if(area > 0)
            {
                overlaps += area;
                _planarGluetags[j]._overlaps = true;
                break;
            }
        }
    }

    return overlaps;
}

double Graph::findTriangleOverlaps()
{
    double overlaps = 0;

    for(size_t i = 0; i < _planarFaces.size(); i++)
    {
        if(_planarFaces[i]._overlaps)
            continue;

        for(size_t j = i + 1; j < _planarFaces.size(); j++)
        {
            if(_planarFaces[j]._overlaps)
                continue;

            if(_planarFaces[i].self == _planarFaces[j].self || _planarFaces[i].parent == _planarFaces[j].self
                    || _planarFaces[i].self == _planarFaces[j].parent || _planarFaces[i].parent == _planarFaces[j].parent)
                continue;

            double area = _planarFaces[j].overlaps(_planarFaces[i]);
            if(area > 0)
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

            // center of the edge
            QVector3D center = (Utility::pointToVector(hfc->prev()->vertex()->point()) + Utility::pointToVector(hfc->vertex()->point())) / 2;

            Edge edge = Edge(faceId, oppositeFaceId, center, hfc, _facets[faceId], _facets[oppositeFaceId]);

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

bool Graph::calculateMSP(std::vector<int> edges)
{
    // clear the previous msp edges
    _mspEdges.clear();
    _cutEdges.clear();

    // create the adjacence list
    std::vector<std::vector<int>> adjacenceList;
    adjacenceList.resize(_facets.size());

    // go through all possible edges
    for(int edge : edges)
    {
        // add edge to msp, add adjacent faces
        _mspEdges.push_back(_edges[ulong(edge)]);
        adjacenceList[ulong(_edges[ulong(edge)]._sFace)].push_back(_edges[ulong(edge)]._tFace);
        adjacenceList[ulong(_edges[ulong(edge)]._tFace)].push_back(_edges[ulong(edge)]._sFace);

        // list storing discovered nodes
        std::vector<bool> discovered(_facets.size());

        // if the MSP is now cyclic, the added egde needs to be removed again
        for(int i = 0; i < int(_facets.size()); i++)
        {
            // if the node is alone (no incident edges), or already discovered, no need to check
            if(adjacenceList[ulong(i)].empty() || discovered[ulong(i)])
                continue;

            // if the graph is cyclic
            if(!isAcyclic(adjacenceList, i, discovered, -1))
            {
                return false;
            }
        }
    }

    if(isSingleComponent(adjacenceList))
    {
        for(Edge edge : _edges)
        {
            if(std::find(_mspEdges.begin(), _mspEdges.end(), edge) == _mspEdges.end())
            {
                _cutEdges.push_back(edge);
            }
        }
        return true;
    }
    else
    {
        return false;
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
        for(int i = 0; i < int(_facets.size()); i++)
        {
            // if the node is alone (no incident edges), or already discovered, no need to check
            if(adjacenceList[ulong(i)].empty() || discovered[ulong(i)])
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

void Graph::calculateGlueTags(std::vector<Gluetag> gluetags)
{
    // clear old gluetags
    _necessaryGluetags.clear();

#ifndef NDEBUG
    std::cout << "Gluetags: " << _cutEdges.size() << std::endl;
    std::cout << "Edges 'to be bent': " << _mspEdges.size() << std::endl;
#endif

    std::vector<bool> tagged;
    tagged.resize(_facets.size());

    for(Gluetag& gluetag : gluetags)
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

    // check if it is acyclic from the first node
    if(!isAcyclic(adjacenceList, 0, discovered, -1))
    {
        return false;
    }

    // if not all nodes have been discovered, the graph is not connected
    for (ulong i = 0; i < discovered.size(); i++)
    {

        // if node at index i was not discovered the graph is not connected
        if(!discovered[i])
        {
#ifndef NDEBUG
            std::cout << "not connected face: " << i << std::endl;
#endif
            return false;
        }
    }
    return true;
}

bool Graph::isAcyclic(std::vector<std::vector<int>> const &adjacenceList, int start, std::vector<bool> &discovered, int parent)
{
    // mark current node as discovered
    discovered[ulong(start)] = true;

    // loop through every edge from (start -> node(s))
    for(int node : adjacenceList[ulong(start)])
    {
        // if this node was not discovered
        if (!discovered[ulong(node)])
        {
            if(!isAcyclic(adjacenceList, node, discovered, int(start))) // start dfs from node
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

void Graph::initEdgeWeight()
{
    // init edge weights
    for(Edge& edge : _edges)
    {
        edge._weight = (double(std::rand()) / RAND_MAX);
    }
    // init gluetag probabilities
    for(Gluetag& gluetag : _gluetags)
    {
        gluetag._probability = (double(std::rand()) / RAND_MAX);
    }
}

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
            if(edge.isInwards)
            {
                QVector3D start = Utility::pointToVector(edge._halfedge->prev()->vertex()->point());
                QVector3D target = Utility::pointToVector(edge._halfedge->vertex()->point());


                QVector3D step = (target - start) / 11; // 10 steps

                for(int i = 0; i < 11; i+=2)
                {
                    lineVertices.push_back(start + i*step);
                    lineVertices.push_back(start + i*step + step);

                    lineColors.push_back(QVector3D(0.0f, 0.0f, 0.0f));
                    lineColors.push_back(QVector3D(0.0f, 0.0f, 0.0f));
                }
            }
            else
            {
                lineVertices.push_back(Utility::pointToVector(edge._halfedge->prev()->vertex()->point()));
                lineVertices.push_back(Utility::pointToVector(edge._halfedge->vertex()->point()));

                lineColors.push_back(QVector3D(0.0f, 0.0f, 0.0f));
                lineColors.push_back(QVector3D(0.0f, 0.0f, 0.0f));
            }
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

void Graph::writeGluetags()
{
    //_bl = QVector3D(float(_edge._halfedge->vertex()->point().x()),
    //                             float(_edge._halfedge->vertex()->point().y()),
    //                             float(_edge._halfedge->vertex()->point().z()));
    // prev

    std::ofstream gluetabs ("model.tab");

    gluetabs << _necessaryGluetags.size() << std::endl;

    for(Gluetag& gt : _necessaryGluetags)
    {
        for ( Polyhedron::Vertex_iterator v = _mesh.vertices_begin(); v != _mesh.vertices_end(); ++v)
        {
            if(gt._edge._halfedge->vertex()->point() == v->point())
            {
                gluetabs << std::distance(_mesh.vertices_begin(), v) << "\t";
            }
        }

        for ( Polyhedron::Vertex_iterator v = _mesh.vertices_begin(); v != _mesh.vertices_end(); ++v)
        {
            if(gt._edge._halfedge->prev()->vertex()->point() == v->point())
            {
                gluetabs << std::distance(_mesh.vertices_begin(), v) << std::endl;
            }
        }

        //auto v = std::find(_mesh.vertices_begin(), _mesh.vertices_end(), gt._edge._halfedge->vertex());
        //gluetabs << std::distance(_mesh.vertices_begin(), v) << "\t";
        //v = std::find(_mesh.vertices_begin(), _mesh.vertices_end(), gt._edge._halfedge->prev()->vertex());
        //gluetabs << std::distance(_mesh.vertices_begin(), v) << std::endl;
    }

    gluetabs.close();
}




void Graph::writeMSP()
{
    std::ofstream transfer ("main.obj");
    transfer << "o Transferable" << std::endl;
    std::stringstream vs;
    std::stringstream vts;

    for (auto& f2p : _CplanarFaces)
    {
        vs << "v" << f2p.A.x() << " " << f2p.A.y() << " " << f2p.A.z() << std::endl;
        vs << "v" << f2p.B.x() << " " << f2p.B.y() << " " << f2p.B.z() << std::endl;
        vs << "v" << f2p.C.x() << " " << f2p.C.y() << " " << f2p.C.z() << std::endl;
        vts << "vt" << f2p.a.x() << " " << f2p.a.y() << std::endl;
        vts << "vt" << f2p.b.x() << " " << f2p.b.y() << std::endl;
        vts << "vt" << f2p.c.x() << " " << f2p.c.y() << std::endl;
    }

    transfer << vs.str() << std::endl;
    transfer << vts.str() << std::endl;
    transfer.close();

    std::ofstream transferGT ("gluetab.obj");

    std::stringstream vsGt;

    for (auto& f2p : _CplanarGluetags)
    {
        vsGt << "v" << f2p.a.x() << " " << f2p.a.y() << " " << std::endl;
        vsGt << "v" << f2p.b.x() << " " << f2p.b.y() << " " << std::endl;
        vsGt << "v" << f2p.c.x() << " " << f2p.c.y() << " " << std::endl;
        vsGt << "v" << f2p.b.x() << " " << f2p.b.y() << " " << std::endl;
        vsGt << "v" << f2p.d.x() << " " << f2p.d.y() << " " << std::endl;
    }

    transferGT << vsGt.str() << std::endl;
    transferGT.close();

    std::ofstream msp ("model.spt");

    for(Edge& edge : _mspEdges)
    {


        /*
        std::cout << "Edge: (" << edge._halfedge->vertex()->point().x() << "," <<
                  edge._halfedge->vertex()->point().y() << "," <<
                  edge._halfedge->vertex()->point().z() << "), ("
                  <<  edge._halfedge->prev()->vertex()->point().x() << ","
                  <<  edge._halfedge->prev()->vertex()->point().y() << ","
                  <<  edge._halfedge->prev()->vertex()->point().z() << ")"
                   << std::endl;
        */
        for (Polyhedron::Edge_iterator e = _mesh.edges_begin(); e != _mesh.edges_end(); ++e)
        {
            /*
            std::cout << "Polyhedron-Iterator: (" << e->vertex()->point().x() << "," <<
                      e->vertex()->point().y() << "," <<
                      e->vertex()->point().z() << "), ("
                      <<  e->prev()->vertex()->point().x() << ","
                      <<  e->prev()->vertex()->point().y() << ","
                      <<  e->prev()->vertex()->point().z() << ")"
                       << std::endl;
            */
            if((edge._halfedge->vertex()->point().x() == e->vertex()->point().x()
            && edge._halfedge->vertex()->point().y() == e->vertex()->point().y()
            && edge._halfedge->vertex()->point().z() == e->vertex()->point().z())

            && (edge._halfedge->prev()->vertex()->point().x() == e->prev()->vertex()->point().x()
            && edge._halfedge->prev()->vertex()->point().y() == e->prev()->vertex()->point().y()
            && edge._halfedge->prev()->vertex()->point().z() == e->prev()->vertex()->point().z()))
            {
                msp << std::distance(_mesh.edges_begin(), e) << std::endl;
                break;
            }
            //if(e->vertex() == edge._halfedge->vertex() && e->prev()->vertex() == edge._halfedge->prev()->vertex())
            //{
            //    msp << std::distance(_mesh.edges_begin(), e) << std::endl;
            //}
        }

        //Polyhedron::Edge_iterator e = std::find(_mesh.edges_begin(), _mesh.edges_end(), edge._halfedge);
        //msp << std::distance(_mesh.edges_begin(), e) << std::endl;
    }

    msp.close();

}

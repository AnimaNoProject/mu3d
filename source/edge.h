class Edge
{
public:
    int _sFace;
    int _tFace;
    double _distance;

    Edge(int sFace, int tFace, double distance);

    bool operator==(const Edge& other) const;
    bool operator<(const Edge& other) const;
};

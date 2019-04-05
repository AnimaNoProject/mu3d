#include <QVector3D>

class Edge
{
public:
    int _sFace;
    int _tFace;
    double _distance;

    QVector3D _middle;

    Edge(int sFace, int tFace, double distance, QVector3D middle);

    bool operator==(const Edge& other) const;
    bool operator<(const Edge& other) const;
};

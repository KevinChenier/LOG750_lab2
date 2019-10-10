#ifndef CUBE_H
#define CUBE_H

#include <node.h>
#include <QVector3D>
#include <QQueue>

class Cube : public Node
{
private:
    QVector3D rootVertice = {0,0,0};
    float dimArret = 0.5f;
public:
    explicit Cube (){}

    QVector3D getRootVertice(){return rootVertice;}

    QQueue<float[3]> getVertices();

    void setRootVertice(QVector3D lrootVertice){ rootVertice = lrootVertice;}

    void setDimArret(float lDimArret){ dimArret = lDimArret;}

    float getDimArret(){return dimArret;}
};

#endif // CUBE_H

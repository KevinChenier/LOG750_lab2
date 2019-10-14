#ifndef CUBE_H
#define CUBE_H

#include <node.h>
#include <QVector3D>
#include <QQueue>
#include <QGLViewer/qglviewer.h>
class Cube : public Node
{
private:
    QVector3D rootVertice = QVector3D(0.0f,0.0f,0.0f);
    float dimArret = 0.5f;
public:
    explicit Cube (){}

    QVector3D getRootVertice(){return rootVertice;}

    QQueue<QVector3D> getVertices();

    void setRootVertice(QVector3D lrootVertice){ rootVertice = lrootVertice;}

    void setDimArret(float lDimArret){ dimArret = lDimArret;}

    float getDimArret(){return dimArret;}
};

#endif // CUBE_H

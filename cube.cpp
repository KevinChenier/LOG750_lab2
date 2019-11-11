#include "cube.h"
#include <QQueue>

Cube::Cube ()
{
}

QQueue<QVector3D> Cube::getVertices()
{
    // source : https://github.com/in2gpu/in2gpu_tutorials/blob/fcdfe647183bf29b44bc9fa7e1b76d7a1dac27a2/in2gpu_tutorials/Chapter_2/c2_2_DrawCubeIndex/CubeIndex.cpp
    QQueue <QVector3D> vertices;
    float dimArretFromCenter = dimArret / 2.f;

    //front
    vertices.append(QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
    vertices.append(QVector3D( dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
    vertices.append(QVector3D( dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
    vertices.append(QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));

    //right
    vertices.append(QVector3D(dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
    vertices.append(QVector3D(dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));
    vertices.append(QVector3D(dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
    vertices.append(QVector3D(dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));

    //back
    vertices.append(QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
    vertices.append(QVector3D( dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
    vertices.append(QVector3D( dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));
    vertices.append(QVector3D(-dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

    //left
    vertices.append(QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
    vertices.append(QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
    vertices.append(QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
    vertices.append(QVector3D(-dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

    //upper
    vertices.append(QVector3D( dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
    vertices.append(QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
    vertices.append(QVector3D(-dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));
    vertices.append(QVector3D( dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

    //bottom
    vertices.append(QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
    vertices.append(QVector3D( dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
    vertices.append(QVector3D( dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
    vertices.append(QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));

    return vertices;
}

const QVector3D Cube::getNormal(int faceID)
{
    switch(faceID%6)
    {
        case 0: return frontN;
        case 1: return rightN;
        case 2: return backN;
        case 3: return leftN;
        case 4: return upperN;
        case 5: return bottomN;
    }
}

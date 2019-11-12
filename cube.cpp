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
        case 0: return QVector3D::normal(getVertices()[0], getVertices()[1], getVertices()[2]);
        case 1: return -QVector3D::normal(getVertices()[4], getVertices()[5], getVertices()[6]);
        case 2: return -QVector3D::normal(getVertices()[8], getVertices()[9], getVertices()[10]);
        case 3: return QVector3D::normal(getVertices()[12], getVertices()[13], getVertices()[14]);
        case 4: return -QVector3D::normal(getVertices()[16], getVertices()[17], getVertices()[18]);
        case 5: return QVector3D::normal(getVertices()[20], getVertices()[21], getVertices()[22]);
    }
}

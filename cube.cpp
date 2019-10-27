#include "cube.h"
#include <QQueue>

Cube::Cube ()
{
}

QQueue<QVector3D> Cube::getVertices()
{
    // source : https://github.com/in2gpu/in2gpu_tutorials/blob/fcdfe647183bf29b44bc9fa7e1b76d7a1dac27a2/in2gpu_tutorials/Chapter_2/c2_2_DrawCubeIndex/CubeIndex.cpp
       QQueue <QVector3D> vertices ;
       QMatrix4x4 t = getTransformation();
       float dimArretFromCenter = dimArret / 2.f;

       //front
       vertices.append(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
       vertices.append(t*QVector3D( dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
       vertices.append(t*QVector3D( dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.append(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));

       //right
       vertices.append(t*QVector3D(dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.append(t*QVector3D(dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));
       vertices.append(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.append(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));

       //back
       vertices.append(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.append(t*QVector3D( dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.append(t*QVector3D( dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));
       vertices.append(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

       //left
       vertices.append(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.append(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
       vertices.append(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.append(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

       //upper
       vertices.append(t*QVector3D( dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.append(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.append(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));
       vertices.append(t*QVector3D( dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

       //bottom
       vertices.append(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.append(t*QVector3D( dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.append(t*QVector3D( dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
       vertices.append(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));

    return vertices;
}

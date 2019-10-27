#include "cube.h"
#include <QQueue>

Cube::Cube ()
{
}

QQueue<QVector3D> Cube::getVertices()
{
    // source : https://github.com/in2gpu/in2gpu_tutorials/blob/fcdfe647183bf29b44bc9fa7e1b76d7a1dac27a2/in2gpu_tutorials/Chapter_2/c2_1_DrawCube/Cube.cpp
       QQueue <QVector3D> vertices ;
       QMatrix4x4 t = getTransformation();
       float dimArretFromCenter = dimArret / 2.f;

       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));

       vertices.push_back(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));


       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));


       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));

       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));

       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

       vertices.push_back(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, dimArretFromCenter, -dimArretFromCenter));

       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));

       vertices.push_back(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, -dimArretFromCenter));
       vertices.push_back(t*QVector3D(-dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));
       vertices.push_back(t*QVector3D(dimArretFromCenter, -dimArretFromCenter, dimArretFromCenter));

    return vertices;
}

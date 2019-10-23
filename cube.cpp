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

       //front
           vertices.append(t*QVector3D(-0.5, -0.5, 0.5));
           vertices.append(t*QVector3D( 0.5, -0.5, 0.5));
           vertices.append(t*QVector3D( 0.5,  0.5, 0.5));
           vertices.append(t*QVector3D(-0.5,  0.5, 0.5));

           //right
           vertices.append(t*QVector3D(0.5,  0.5,   0.5));
           vertices.append(t*QVector3D(0.5,  0.5,  -0.5));
           vertices.append(t*QVector3D(0.5, -0.5,  -0.5));
           vertices.append(t*QVector3D(0.5, -0.5,   0.5));

           //back
           vertices.append(t*QVector3D(-0.5, -0.5, -0.5));
           vertices.append(t*QVector3D( 0.5, -0.5, -0.5));
           vertices.append(t*QVector3D( 0.5,  0.5, -0.5));
           vertices.append(t*QVector3D(-0.5,  0.5, -0.5));

           //left
           vertices.append(t*QVector3D(-0.5, -0.5, -0.5));
           vertices.append(t*QVector3D(-0.5, -0.5,  0.5));
           vertices.append(t*QVector3D(-0.5,  0.5,  0.5));
           vertices.append(t*QVector3D(-0.5,  0.5, -0.5));

           //upper
           vertices.append(t*QVector3D( 0.5, 0.5,  0.5));
           vertices.append(t*QVector3D(-0.5, 0.5,  0.5));
           vertices.append(t*QVector3D(-0.5, 0.5, -0.5));
           vertices.append(t*QVector3D( 0.5, 0.5, -0.5));
           //bottom
           vertices.append(t*QVector3D(-0.5, -0.5, -0.5));
           vertices.append(t*QVector3D( 0.5, -0.5, -0.5));
           vertices.append(t*QVector3D( 0.5, -0.5,  0.5));
           vertices.append(t*QVector3D(-0.5, -0.5,  0.5));

    return vertices;
}

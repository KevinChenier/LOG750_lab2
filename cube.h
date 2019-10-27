#ifndef CUBE_H
#define CUBE_H

#include <node.h>
#include <QVector3D>
#include <QQueue>
#include <QGLViewer/qglviewer.h>

class Cube : public Node
{
public:
    // Constructor
    explicit Cube ();

    // Getters
    QQueue<QVector3D> getVertices() override ;
   // QQueue<Cube> getQueueCube() { return queueCube; }
    //Cube getChild(int index) { Cube n = queueCube[index]; return n; }

    //void addChild(Cube child) { queueCube.append(child); }

    // source : https://github.com/in2gpu/in2gpu_tutorials/blob/fcdfe647183bf29b44bc9fa7e1b76d7a1dac27a2/in2gpu_tutorials/Chapter_2/c2_2_DrawCubeIndex/CubeIndex.cpp
    GLint indices [36] =
    {
        0,  1,  2,  0,  2,  3,   //front
        4,  5,  6,  4,  6,  7,   //right
        8,  9,  10, 8,  10, 11,  //back
        12, 13, 14, 12, 14, 15,  //left
        16, 17, 18, 16, 18, 19,  //upper
        20, 21, 22, 20, 22, 23 //bottom
    };

    QVector3D Normales[24]={
        frontN, frontN, frontN, frontN,   //front
        rightN, rightN, rightN, rightN,   //right
        backN, backN, backN,    backN,  //back
        leftN, leftN, leftN, leftN,  //left
        upperN, upperN, upperN, upperN,  //upper
        bottomN, bottomN, bottomN, bottomN, //bottom
    };
    static constexpr float dimArret = 1.0f;

};

#endif // CUBE_H

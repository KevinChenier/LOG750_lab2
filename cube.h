#ifndef CUBE_H
#define CUBE_H

#include <node.h>
#include <QVector3D>
#include <QQueue>
#include <QGLViewer/qglviewer.h>
class Cube : public Node
{
private:
    QVector3D rootVertice = QVector3D(-1.0f,-1.0f,1.0f);
    float dimArret = 0.5f;
    QQueue<Cube> queueCube;

public:
    explicit Cube (){}
    explicit Cube (float ldimArret){ dimArret = ldimArret ;}
    QVector3D getRootVertice(){return rootVertice;}

    QQueue<QVector3D> getVertices();

    QQueue<Cube> getQueueCube(){return queueCube;}

    void addChild(Cube child){queueCube.append(child);}

    Cube getChild(int index){Cube n = queueCube[index]; return n;}

    void setRootVertice(QVector3D lrootVertice){ rootVertice = lrootVertice;}

    void setDimArret(float lDimArret){ dimArret = lDimArret;}

    float getDimArret(){return dimArret;}

    // source : https://doc.qt.io/qt-5/qtopengl-cube-example.html
    GLint indices [34] = {
        0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };
};

#endif // CUBE_H

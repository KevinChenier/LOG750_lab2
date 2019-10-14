#include "cube.h"
#include <QQueue>
QQueue<QVector3D> Cube::getVertices(){
   // source : https://doc.qt.io/qt-5/qtopengl-cube-example.html
    QQueue <QVector3D> vertices ;
    vertices.append(rootVertice); //vo
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()));//v1
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y(),rootVertice.z()+dimArret));//v2
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()+dimArret));//v3

    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()));//v4
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()));//v5
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()+dimArret));//v6
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()+dimArret));//v7

    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()));//v8
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()));//v9
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()+dimArret));//v10
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()+dimArret));//v11

    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()));//v12
    vertices.append(rootVertice);//v13
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()+dimArret));//v14
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y(),rootVertice.z()+dimArret));//v15

    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()));//v16
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()));//v17
    vertices.append(rootVertice);//v18
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()));//v19

    vertices.append(QVector3D(rootVertice.x(),rootVertice.y(),rootVertice.z()+dimArret));//v20
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()+dimArret));//v21
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()+dimArret));//v22
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()+dimArret));//v23
            return  vertices ;
}

#include "cube.h"
#include <QQueue>
QQueue<QVector3D> Cube::getVertices(){
   // source : https://doc.qt.io/qt-5/qtopengl-cube-example.html
    QQueue <QVector3D> vertices ;
    vertices.append(rootVertice); //vo
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()));//v1
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()));//v2
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()));//v3

    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()));//v4
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()-dimArret));//v5
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()));//v6
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()- dimArret));//v7

    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()- dimArret));//v8
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y(),rootVertice.z()-dimArret));//v9
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()-dimArret));//v10
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()-dimArret));//v11

    vertices.append(QVector3D(rootVertice.x(),rootVertice.y(),rootVertice.z()-dimArret));//v12
    vertices.append(rootVertice);//v13
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()-dimArret));//v14
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()));//v15

    vertices.append(QVector3D(rootVertice.x(),rootVertice.y(),rootVertice.z()-dimArret));//v16
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()-dimArret));//v17
    vertices.append(rootVertice);//v18
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()));//v19

    vertices.append(QVector3D(rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()));//v20
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()));//v21
    vertices.append(QVector3D(rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()-dimArret));//v22
    vertices.append(QVector3D(rootVertice.x()+dimArret,rootVertice.y()+dimArret,rootVertice.z()-dimArret));//v23
            return  vertices ;
}

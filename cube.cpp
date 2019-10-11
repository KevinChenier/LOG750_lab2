#include "cube.h"
#include <QQueue>
QQueue<QVector3D> Cube::getVertices(){
    QQueue <QVector3D> vertices ;
    vertices.append({rootVertice.x(),rootVertice.y(),rootVertice.z()});
    vertices.append({rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()});
    vertices.append({rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()});
            return  vertices ;
}

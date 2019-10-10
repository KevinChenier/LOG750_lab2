#include "cube.h"
#include <QQueue>
QQueue<float[3]> Cube::getVertices(){
    QQueue <float[3]> vertices ;
    vertices.append({rootVertice.x(),rootVertice.y(),rootVertice.z()});
    vertices.append({rootVertice.x()+dimArret,rootVertice.y(),rootVertice.z()});
    vertices.append({rootVertice.x(),rootVertice.y()+dimArret,rootVertice.z()});
            return  vertices ;
}

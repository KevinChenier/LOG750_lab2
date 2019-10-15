#include <node.h>
QMatrix4x4 Node::getTransformation(){
    QMatrix4x4 t;
    t.setToIdentity();
    while (!stackTransformation.isEmpty()){
        t*=stackTransformation.pop();
    }
    return t ;
}


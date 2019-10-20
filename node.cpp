#include <node.h>

Node::Node()
{
    QMatrix4x4 tmpTransformation ;
    tmpTransformation.setToIdentity();
    stackTransformation.append(tmpTransformation);
}

QMatrix4x4 Node::getTransformation()
{
    QMatrix4x4 t;
    t.setToIdentity();

    while (!stackTransformation.isEmpty()) {
        t*=stackTransformation.pop();
    }
    return t;
}


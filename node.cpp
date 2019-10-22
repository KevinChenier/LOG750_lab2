#include <node.h>

Node::Node()
{
    QMatrix4x4 tmpTransformation ;
    tmpTransformation.setToIdentity();
    stackTransformation.append(tmpTransformation);
}

QMatrix4x4 Node::getTransformation()
{
    QMatrix4x4 transformation;
    transformation.setToIdentity();

    while (!stackTransformation.isEmpty()) {
        transformation*=stackTransformation.pop();
    }
    return transformation;
}


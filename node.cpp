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

    int size = stackTransformation.length();
    for (int var = size - 1; var >= 0; --var) {
         transformation*=stackTransformation[var];
    }
    return transformation;
}

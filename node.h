#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QQueue>
#include <QStack>

class Node
{
public:
    // Constructor
    explicit Node();

    // Getters
    QMatrix4x4 getTransformation();
    QStack<QMatrix4x4> getStackTransformation() { return stackTransformation; }

    void addTransformation(QMatrix4x4 tmpTransformation) { stackTransformation.append(tmpTransformation); }

private:
    QStack<QMatrix4x4> stackTransformation;
};

#endif // NODE_H

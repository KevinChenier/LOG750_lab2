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
    virtual QQueue<QVector3D> getVertices() {}
    QMatrix4x4 getTransformation();
    QStack<QMatrix4x4> getStackTransformation() { return stackTransformation; }
    QQueue<Node*> getNodes() { return nodes; }
    Node* getChild(int index) { return nodes.at(index); }

    void addChild(Node* child) { nodes.append(child); }

    void addTransformation(QMatrix4x4 tmpTransformation) {
        stackTransformation.append(tmpTransformation);
        int size = nodes.length();
        for (int var = 0; var < size; ++var) {
            QMatrix4x4 t(tmpTransformation);
            nodes.at(var)->addTransformation(t);
        }
    }

private:
    QStack<QMatrix4x4> stackTransformation;

protected:
    QQueue<Node*> nodes;

};

#endif // NODE_H

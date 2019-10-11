/****************************************************************************

 Copyright (C) 2002-2008 Gilles Debunne. All rights reserved.

 This file is part of the QGLViewer library version 2.3.6.

 http://www.libqglviewer.com - contact@libqglviewer.com

 This file may be used under the terms of the GNU General Public License 
 versions 2.0 or 3.0 as published by the Free Software Foundation and
 appearing in the LICENSE file included in the packaging of this file.
 In addition, as a special exception, Gilles Debunne gives you certain 
 additional rights, described in the file GPL_EXCEPTION in this package.

 libQGLViewer uses dual licensing. Commercial/proprietary software must
 purchase a libQGLViewer Commercial License.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#ifndef NODE_H
#define NODE_H

#include <QMatrix4x4>
#include <QQueue>



class Node
{


private:
    QMatrix4x4 t;
    QQueue<Node> queue;

public:
    explicit Node(){t.setToIdentity();}
    void addChild(Node child){queue.append(child);}
    // Need update or test
    Node getChild(int index){Node n = queue[index]; return n;}
    QMatrix4x4 getT(){return t;}
    void setT(QMatrix4x4 lT){t=lT;}

};

#endif // NODE_H

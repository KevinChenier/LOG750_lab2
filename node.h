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
#include <QStack>



class Node
{


private:
    QStack<QMatrix4x4> stackTransformation;


public:
    explicit Node(){
        QMatrix4x4 tmpTransformation ;
        tmpTransformation.setToIdentity();
        stackTransformation.append(tmpTransformation);
    }

    QMatrix4x4 getTransformation();
    void addT(QMatrix4x4 tmpTransformation){stackTransformation.append(tmpTransformation);}
    QStack<QMatrix4x4> getStackTransformation(){return stackTransformation ;}

};

#endif // NODE_H

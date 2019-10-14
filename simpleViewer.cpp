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

#include "simpleViewer.h"

#include <QOpenGLShaderProgram>
#include <QMouseEvent>

#include <iostream>
using namespace std;

#include <time.h>
#include <cube.h>
#include <QVector3D>
#include <QQueue>


#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace
{
 // source : https://doc.qt.io/qt-5/qtopengl-cube-example.html
  const int numVerticePerCube = 24 ;
        int numCube = 1;
  const int numIndice = 34 ;
  const int nbFace = 6 ;
  const int nbGrille = 2;
   int numVerticesCubes =numCube*numVerticePerCube;
   int numIndiceCubes = numIndice*numCube;



}

Viewer::Viewer()
{}

Viewer::~Viewer()
{
  cleanup();
}

void Viewer::cleanup()
{
  makeCurrent();

  // Delete shaders
    delete m_programRender;
  m_programRender = nullptr;

  // Delete buffers
  glDeleteBuffers(NumBuffers, m_Buffers);
  glDeleteVertexArrays(NumVAOs, m_VAOs);

  doneCurrent();
}

void Viewer::draw()
{
  // Bind our vertex/fragment shaders
    m_programRender->bind();

  // Get projection and camera transformations
  QMatrix4x4 projectionMatrix;
  QMatrix4x4 modelViewMatrix;
  camera()->getProjectionMatrix(projectionMatrix);
  camera()->getModelViewMatrix(modelViewMatrix);

  m_programRender->setUniformValue(m_projMatrixLocation, projectionMatrix);
  m_programRender->setUniformValue(m_mvMatrixLocation, modelViewMatrix);
  m_programRender->setUniformValue(m_normalMatrixLocation, modelViewMatrix.normalMatrix());

  // Draw the sphere
  // Note: Because we are using an index buffer, we need to call glDrawElements instead
  // of glDrawArrays.
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glBindVertexArray(m_VAOs[VAO_Sphere]);
  glDrawElements(GL_TRIANGLE_STRIP, numIndice * numCube, GL_UNSIGNED_INT, nullptr);
}

void Viewer::init()
{
    setMouseBinding(Qt::ShiftModifier, Qt::LeftButton, CAMERA, NO_MOUSE_ACTION);

    // Initialize openGL
  connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Viewer::cleanup);
  initializeOpenGLFunctions();

    // Init shaders
    initRenderShaders();

    // Create our VertexArrays Objects and VertexBuffer Objects
    glGenVertexArrays(NumVAOs, m_VAOs);
    glGenBuffers(NumBuffers, m_Buffers);

  initGeometrySphere();

    // Init GL properties
    glPointSize(10.0f);
}

void Viewer::initRenderShaders()
{
    // Load vertex and fragment shaders
    m_programRender = new QOpenGLShaderProgram;
  if (!m_programRender->addShaderFromSourceFile(QOpenGLShader::Vertex, "basicShader.vert")) {
        cerr << "Unable to load Shader" << endl
                 << "Log file:" << endl;
        qDebug() << m_programRender->log();
    }
  if (!m_programRender->addShaderFromSourceFile(QOpenGLShader::Fragment, "basicShader.frag")) {
        cerr << "Unable to load Shader" << endl
                 << "Log file:" << endl;
        qDebug() << m_programRender->log();
    }
    m_programRender->link();
    m_programRender->bind();	// Note: This is equivalent to glUseProgram(programId());

    // Specify shader input paramters
    // The strings "vPosition", "mvMatrix", etc. have to match an attribute name in the vertex shader.
    if ((m_vPositionLocation = m_programRender->attributeLocation("vPosition")) < 0)
        qDebug() << "Unable to find shader location for " << "vPosition";

    if ((m_vNormalLocation = m_programRender->attributeLocation("vNormal")) < 0)
        qDebug() << "Unable to find shader location for " << "vNormal";

    if ((m_mvMatrixLocation = m_programRender->uniformLocation("mvMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "mvMatrix";

    if ((m_projMatrixLocation = m_programRender->uniformLocation("projMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "projMatrix";

    if ((m_normalMatrixLocation = m_programRender->uniformLocation("normalMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "normalMatrix";
}

void Viewer::initGeometrySphere()
{
  // Note: To ease the sphere creation, we use an index (aka elements) buffer. This allows us to create
  //			 each vertex once. Afterward, faces are created by specifying the index of the three vertices
  //			 inside the index buffer. For example, a 2D quad could be drawn using the following vertex and
  //       index buffers:
  //
  //			 vertices[4][2] = {{-1,-1},{1,-1},{1,1},{-1,1}};
  //       indices[2*3] = {0, 1, 3, 1, 2, 3};
  //
  //       In this example, the vertices buffer contains 4 vertices, and the indices buffer contains two
  //       triangles formed by the vertices (vertices[0], vertices[1], vertices[3]) and (vertices[1],
  //       vertices[2], vertices[3]) respectively.
  //
  //       Also note that indices are stored in a different type of buffer called Element Array Buffer.

  // Create sphere vertices and faces
  GLfloat vertices[numVerticesCubes][3];
  GLfloat normals[numVerticesCubes][3];
  // need to implement
  GLint indices[numIndiceCubes];

  // Generate surrounding vertices
  int numVertices = 0;
  QQueue<Cube> queueCube;
  queueCube.append(Cube());

  for (int i=0; i<numCube; ++i)
  {
    if (numVertices<numVerticesCubes){
        QQueue<QVector3D> cubeVertices = queueCube[i].getVertices() ;
        while (!cubeVertices.isEmpty()){
            QVector3D currentVertice = cubeVertices.dequeue();
            vertices[numVertices][0] =currentVertice.x();
            vertices[numVertices][1] =currentVertice.y();
            vertices[numVertices][2] =currentVertice.z();

            QVector3D currentVerticeN = currentVertice.normalized();
            normals[numVertices][0] =currentVerticeN.x();
            normals[numVertices][1] =currentVerticeN.y();
            normals[numVertices][2] =currentVerticeN.z();



            // to deleted
            //start
            qInfo() << "vertice " << numVertices ;
            qInfo() << QString::number(vertices[numVertices][0]);
            qInfo() << QString::number(vertices[numVertices][1]);
            qInfo() << QString::number(vertices[numVertices][2]);
            //end

            numVertices++;
        }
    }

  }

  for (int i=0; i<numIndiceCubes; ++i)
  {
    indices[i] = Cube().indices[i/numCube];
    qInfo() << "indec " << i ;
    qInfo() << QString::number(indices[i]);
  }




  // Fill vertex VBO
  GLsizeiptr offsetVertices = 0;
  GLsizeiptr offsetNormals = sizeof(vertices);
  GLsizeiptr dataSize = offsetNormals + GLsizeiptr(sizeof(normals));

  glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VBO_Sphere]);
  glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, offsetVertices, sizeof(vertices), vertices);
  glBufferSubData(GL_ARRAY_BUFFER, offsetNormals, sizeof(normals), normals);

  // Set VAO
  glBindVertexArray(m_VAOs[VAO_Sphere]);
  glVertexAttribPointer(GLuint(m_vPositionLocation), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetVertices));
  glEnableVertexAttribArray(GLuint(m_vPositionLocation));

  glVertexAttribPointer(GLuint(m_vNormalLocation), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetNormals));
  glEnableVertexAttribArray(GLuint(m_vNormalLocation));

  // Fill in indices EBO
  // Note: The current VAO will remember the call to glBindBuffer for a GL_ELEMENT_ARRAY_BUFFER.
  //			 However, we will need to call glDrawElements() instead of glDrawArrays().
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[EBO_Sphere]);
  // need add indices
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
   glClearColor(0.5f, 0.5f, 0.5f, 1.0);// add background
}

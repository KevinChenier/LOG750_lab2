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

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

namespace
{
  const int numRowSphere = 20;
  const int numColSphere = numRowSphere+2;
  const int numVerticesSphere = numColSphere * numRowSphere + 2;
  const int numTriSphere = numColSphere*(numRowSphere-1)*2 + 2*numColSphere;
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
  glBindVertexArray(m_VAOs[VAO_Sphere]);
  glDrawElements(GL_TRIANGLES, numTriSphere*3, GL_UNSIGNED_INT, nullptr);
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
  GLfloat vertices[numVerticesSphere][3];
  GLfloat normals[numVerticesSphere][3];
  GLint indices[numTriSphere*3][3];

  // Generate surrounding vertices
  unsigned int v = 0;
  float thetaInc = 2.0f*3.14159265f / static_cast<float>(numColSphere);
  float phiInc = 3.14159265f / static_cast<float>(numRowSphere+1);
  for (int row=0; row<numRowSphere; ++row)
  {
    float phi = 3.14159265f - (static_cast<float>(row+1) * phiInc);
    for (int col=0; col<numColSphere; ++col, ++v)
    {
      float theta = col*thetaInc;
      vertices[v][0] = 0.5f*sin(theta)*sin(phi);
      vertices[v][1] = 0.5f*cos(phi);
      vertices[v][2] = 0.5f*cos(theta)*sin(phi);

      normals[v][0] = vertices[v][0]*2.0f;	// Multiply by 2 because sphere radius is 0.5
      normals[v][1] = vertices[v][1]*2.0f;
      normals[v][2] = vertices[v][2]*2.0f;
    }
  }

  // Generate cap vertices
  vertices[numColSphere*numRowSphere+0][0] = 0.0f;
  vertices[numColSphere*numRowSphere+0][1] = -0.5f;
  vertices[numColSphere*numRowSphere+0][2] = 0.0f;

  vertices[numColSphere*numRowSphere+1][0] = 0.0f;
  vertices[numColSphere*numRowSphere+1][1] = 0.5f;
  vertices[numColSphere*numRowSphere+1][2] = 0.0f;

  normals[numColSphere*numRowSphere+0][0] = 0.0f;
  normals[numColSphere*numRowSphere+0][1] = -1.0f;
  normals[numColSphere*numRowSphere+0][2] = 0.0f;

  normals[numColSphere*numRowSphere+1][0] = 0.0f;
  normals[numColSphere*numRowSphere+1][1] = 1.0f;
  normals[numColSphere*numRowSphere+1][2] = 0.0f;

  // Generate surrounding indices (faces)
  unsigned int tri = 0;
  for (int row=0; row<numRowSphere-1; ++row)
  {
    int rowStart = row*numColSphere;
    int topRowStart = rowStart + numColSphere;

    for (int col=0; col<numColSphere; ++col, tri += 2)
    {
      // Compute quad vertices
      int v = rowStart + col;
      int vi = (col<numColSphere-1) ? v+1 : rowStart;
      int vj = topRowStart + col;
      int vji = (col<numColSphere-1) ? vj+1 : topRowStart;

      // Add to indices
      indices[tri+0][0] = v;
      indices[tri+0][1] = vi;
      indices[tri+0][2] = vj;
      indices[tri+1][0] = vi;
      indices[tri+1][1] = vji;
      indices[tri+1][2] = vj;
    }
  }

  // Generate cap indices (faces)
  for (int col=0; col<numColSphere; ++col, tri += 2)
  {
    indices[tri+0][0] = numColSphere*numRowSphere;
    indices[tri+0][1] = (col<numColSphere-1) ? col+1 : 0;
    indices[tri+0][2] = col;

    int rowStart = (numRowSphere-1)*numColSphere;
    indices[tri+1][0] = numColSphere*numRowSphere+1;
    indices[tri+1][1] = rowStart + col;
    indices[tri+1][2] = (col<numColSphere-1) ? (rowStart + col + 1) : rowStart;
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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

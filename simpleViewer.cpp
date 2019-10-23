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
    const int numVerticePerCube = 24;
    const int numCubesPerRow = 10;
    const int numCubesPerCol = 10;
    int numCubes = numCubesPerRow * numCubesPerCol;
    const int numIndicePerCube = 36;
    int numVertices = numCubes * numVerticePerCube;
    int numIndices = numCubes * numIndicePerCube;

    // rootCube is not the first cube.
    QQueue<Cube> graph ;
}

Viewer::Viewer()
    : m_selectedCube(-1)
{
}

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

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(m_VAOs[VAO_Cube]);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, nullptr);
}

void Viewer::init()
{
    setMouseBinding(Qt::ShiftModifier, Qt::LeftButton, CAMERA, NO_MOUSE_ACTION);

    // Initialize openGL
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Viewer::cleanup);
    initializeOpenGLFunctions();

    // Init shaders
    initRenderShaders();
    initPickingShaders();

    // Create our VertexArrays Objects and VertexBuffer Objects
    glGenVertexArrays(NumVAOs, m_VAOs);
    glGenBuffers(NumBuffers, m_Buffers);
    setSceneRadius(10);          // scene has a 10 OpenGL units radius
    camera()->showEntireScene();
    initScene();
    initGeometryCube();

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

void Viewer::initPickingShaders()
{
    // Load vertex and fragment shaders
    m_programPicking = new QOpenGLShaderProgram;
    if (!m_programPicking->addShaderFromSourceFile(QOpenGLShader::Vertex, "constantColor.vert")) {
        cerr << "Unable to load Shader" << endl
                 << "Log file:" << endl;
        qDebug() << m_programPicking->log();
    }
    if (!m_programPicking->addShaderFromSourceFile(QOpenGLShader::Fragment, "constantColor.frag")) {
        cerr << "Unable to load Shader" << endl
                 << "Log file:" << endl;
        qDebug() << m_programPicking->log();
    }
    m_programPicking->link();
    m_programPicking->bind();	// Note: This is equivalent to glUseProgram(programId());

    // Specify shader input paramters
    // The strings "vPosition", "mvMatrix", etc. have to match an attribute name in the vertex shader.
    if ((m_vPositionLocationPicking = m_programPicking->attributeLocation("vPosition")) < 0)
        qDebug() << "Unable to find shader location for " << "vPosition";

    if ((m_colorLocationPicking = m_programPicking->uniformLocation("color")) < 0)
        qDebug() << "Unable to find shader location for " << "color";

    if ((m_mvMatrixLocationPicking = m_programPicking->uniformLocation("mvMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "mvMatrix";

    if ((m_projMatrixLocationPicking = m_programPicking->uniformLocation("projMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "projMatrix";
}

void Viewer::initGeometryCube()
{
    // Create vertices, faces and indices
    GLfloat vertices[numVertices][3];
    GLfloat normals[numVertices][3];
    GLint indices[numIndices];

    // Generate surrounding vertices
    int v = 0;

    //IMPORTANT: rootCube is not the first cube.
    //IMPORTANT: TODO getAllVertices(numVertices) on rootCube shoulbe return
    // numVertices include vertices of child (see pattern vistor)
    // Don't forget to update numCubes after adding a cube when the application is up
    Cube tmpCube = Cube() ;
    //QQueue<Cube> childrenRootCube = tmpCube.getQueueCube(); //QQueue<QVector3D> vertices = rootNode.getAllVertices(numVertices) ;

    /*
    // TODO: write recursive method with pattern visitor to get getAllVertices(numVertices)

    while (!vertices.isEmpty()){
        QVector3D currentVertice = vertices.dequeue();
        vertices[v][0] =currentVertice.x();
        vertices[v][1] =currentVertice.y();
        vertices[v][2] =currentVertice.z();

        QVector3D currentVerticeN = currentVertice.normalized();
        normals[v][0] =currentVerticeN.x();
        normals[v][1] =currentVerticeN.y();
        normals[v][2] =currentVerticeN.z();

        v++;
    }
    */

    // With good conception this line will be deleted
    int numCubeInRootCube = graph.length();
    for (int i=0; i<numCubes; ++i)
    {
        if (v<numVertices && i< numCubeInRootCube){

            Cube currentCube = graph.at(i) ;
             QQueue<QVector3D> cubeVertices = currentCube.getVertices() ;

            while (!cubeVertices.isEmpty()) {
                QVector3D currentVertice = cubeVertices.dequeue();
                QVector3D currentNormal =  tmpCube.Normales[v%numVerticePerCube] ;

                vertices[v][0] =currentVertice.x();
                vertices[v][1] =currentVertice.y();
                vertices[v][2] =currentVertice.z();

                normals[v][0] = currentNormal.x();
                normals[v][1] =currentNormal.y();
                normals[v][2] =currentNormal.z();

                //qInfo() << "vertices" << v;
                //qInfo() << QString::number(vertices[v][0]);
                //qInfo() << QString::number(vertices[v][1]);
                //qInfo() << QString::number(vertices[v][2]);

                v++;
            }
        }
    }

    for (int i=0; i<numIndices; ++i)
    {
        indices[i] = tmpCube.indices[i%numIndicePerCube]+(i/numIndicePerCube)*numVerticePerCube ;

        //qInfo() << "indec " << i ;
        //qInfo() << QString::number(indices[i]);
    }

    // Fill vertex VBO
    GLsizeiptr offsetVertices = 0;
    GLsizeiptr offsetNormals = sizeof(vertices);
    GLsizeiptr dataSize = offsetNormals + GLsizeiptr(sizeof(normals));

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VBO_Cube]);
    glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, offsetVertices, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offsetNormals, sizeof(normals), normals);

    // Set VAO
    glBindVertexArray(m_VAOs[VAO_Cube]);

    glVertexAttribPointer(GLuint(m_vPositionLocation), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetVertices));
    glEnableVertexAttribArray(GLuint(m_vPositionLocation));

    glVertexAttribPointer(GLuint(m_vNormalLocation), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetNormals));
    glEnableVertexAttribArray(GLuint(m_vNormalLocation));

    // Fill in indices EBO
    // Note: The current VAO will remember the call to glBindBuffer for a GL_ELEMENT_ARRAY_BUFFER.
    // However, we will need to call glDrawElements() instead of glDrawArrays().
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[EBO_Cube]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Create VAO for cubes during picking (with constant shader)
    glBindVertexArray(m_VAOs[VAO_CubesPicking]);

    glVertexAttribPointer(GLuint(m_vPositionLocationPicking), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetVertices));
    glEnableVertexAttribArray(GLuint(m_vPositionLocationPicking));

    glClearColor(0.5f, 0.5f, 0.5f, 1.0);// add background
}

void Viewer::initScene()
{
    const float dimArret = Cube::dimArret;
    for (int i=0; i < numCubesPerRow; ++i)
    {
        for (int j=0; j < numCubesPerCol; ++j)
        {
            Cube currentCube = Cube();
            QMatrix4x4 cubeTranformation ;
            cubeTranformation.translate(QVector3D(i*dimArret, 0, j*dimArret));
            currentCube.addTransformation(cubeTranformation);
            graph.append(currentCube);
        }
    }
}

void Viewer::mousePressEvent(QMouseEvent *e)
{
    QGLViewer::mousePressEvent(e);
    std::cout << "Viewer::mousePressEvent" << std::endl;

    if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ShiftModifier))
    {
        performSelection(e->x(), e->y());
        update();
    }
}

void Viewer::performSelection(int x, int y)
{
    // Map (dictionnary) used to store the correspondences between colors and spiral number.
    QMap<unsigned int, int> myMap;
    // Identificator ID used as a color for rendering and as a key for the map.
    QRgb id = 0;

    makeCurrent();	// This allows us to use OpenGL functions outside of Viewer::draw()

    std::cout << "Viewer::performSelection(" << x << ", " << y << ")" << std::endl;

    // Selection is performed by drawing the spirals with a color that matches their ID
    // Note: Because we are drawing outside the draw() function, the back buffer is not
    //       swapped after this function is called.

    // Clear back buffer. Since we set a differet clear color, we save the
    // previous value and restore it after the backbuffer has been cleared.
    float clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);

    glClearColor(1,1,1,1);	// This value should not match any existing index
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    // Bind our vertex/fragment shaders
    m_programPicking->bind();

    // Get projection and camera transformations
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 modelViewMatrix;
    camera()->getProjectionMatrix(projectionMatrix);
    camera()->getModelViewMatrix(modelViewMatrix);

    const float dimArret = Cube::dimArret;

    // Draw the cubes
    glBindVertexArray(m_VAOs[VAO_CubesPicking]);
    m_programPicking->setUniformValue(m_projMatrixLocationPicking, projectionMatrix);
    for (int i=0; i < numCubesPerRow; ++i)
    {
        for (int j=0; j < numCubesPerCol; ++j)
        {
            // Save transformations
            QMatrix4x4 originalModelViewMatrix(modelViewMatrix);

            // Translate cube
            modelViewMatrix.translate(QVector3D(i*dimArret, 0, j*dimArret));

            // For convenience, convert the ID to a color object.
            QColor color;
            color.setRgba(id);
            // Get the equivalent of the color as an unsigned long.
            unsigned int key = color.rgba();
            // Insert the key (unsigned long) in the map.
            myMap.insert(key, id);
            // Set the color value for the shader.
            m_programPicking->setUniformValue(m_colorLocationPicking, color);

            // Draw the cubes
            m_programPicking->setUniformValue(m_mvMatrixLocationPicking, modelViewMatrix);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, numVerticePerCube);

            // Restore previous transformations
            modelViewMatrix = originalModelViewMatrix;
            // Increment the ID, as it is not in use.
            id++;
        }
    }
    // Wait until all drawing commands are done
    glFinish();

    // Read the pixel under the cursor
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    unsigned char pixelData[4];
    glReadPixels(x, camera()->screenHeight()-1-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

    std::cout << "Selected pixelData: " << int(pixelData[0]) << ", "
                                                                            << int(pixelData[1]) << ", "
                                                                            << int(pixelData[2]) << ", "
                                                                            << int(pixelData[3]) << std::endl;

    // For convenience, construct a color object matching what was read in the frame buffer.
    QColor pickedColor(pixelData[0], pixelData[1], pixelData[2], pixelData[3]);
    // Get the equivalent of the color as an unsigned int. This is the key we stored earlier in the map.
    unsigned int key = pickedColor.rgba();
    // Get the value matching the key.
    int value = myMap.value(key, -1);
    m_selectedCube = value;
    std::cout << "m_selectedCube: " << m_selectedCube << std::endl;

    // We are done with OpenGL
    doneCurrent();
}

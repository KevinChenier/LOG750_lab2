#include "simpleViewer.h"

#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QOpenGLFramebufferObject>

#include <iostream>
#include "OBJLoader.h"
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
    QMatrix4x4 toolTransform;

    // rootCube is not the first cube.
    QQueue<Cube*> graph;

    const int ShadowSizeX = 2048;
    const int ShadowSizeY = 2048;
}

Viewer::Viewer()
    : m_selectedFace(-1),
      m_selectedCubeOnClick(-1)
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
    if(m_programRender!=nullptr) {
        delete m_programRender;
        m_programRender = nullptr;
    }
    if (m_shadowMapShader != nullptr) {
        delete m_shadowMapShader;
        m_shadowMapShader = nullptr;
    }
    if(m_programPicking!=nullptr) {
        delete m_programPicking;
        m_programPicking = nullptr;
    }

    // Delete buffers
    glDeleteBuffers(NumBuffers, m_Buffers);
    glDeleteVertexArrays(NumVAOs, m_VAOs);

    // Delete mesh
    for (unsigned int i=0; i<_meshesGL.size(); ++i)
    {
        glDeleteVertexArrays(1, &_meshesGL[i].vao);
        glDeleteBuffers(1, &_meshesGL[i].vbo);
    }
    _meshesGL.clear();

    // Delete textures
    if (m_textureColor!=nullptr) {
        delete m_textureColor;
        m_textureColor = nullptr;
    }
    if (m_textureNormal != nullptr) {
        delete m_textureNormal;
        m_textureNormal = nullptr;
    }

    // Delete shadow map
    if (m_shadowFBO != nullptr) {
        delete m_shadowFBO;
        m_shadowFBO = nullptr;
    }
    doneCurrent();
}

void Viewer::draw()
{
    // Shadow pass.
    shadowRender();

    // Clear buffers.
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Bind our vertex/fragment shaders
    m_programRender->bind();

    // Get projection and camera transformations
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 modelViewMatrix;
    QMatrix4x4 viewMatrix;
    camera()->getProjectionMatrix(projectionMatrix);
    camera()->getModelViewMatrix(modelViewMatrix);
    camera()->getModelViewMatrix(viewMatrix);

    m_programRender->setUniformValue(m_projMatrixLocation, projectionMatrix);
    m_programRender->setUniformValue(m_mvMatrixLocation, modelViewMatrix);
    m_programRender->setUniformValue(m_viewMatrixLocation, viewMatrix);
    m_programRender->setUniformValue(m_normalMatrixLocation, modelViewMatrix.normalMatrix());
    m_programRender->setUniformValue(m_spotLightPositionLocation, m_spotLightPosition);
    m_programRender->setUniformValue(m_spotLightDirectionLocation, m_spotLightDirection);
    m_programRender->setUniformValue(m_lightMvpMatrixLoc, m_lightViewProjMatrix);

    // Bind the shadow depth map to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_shadowFBO->texture());

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(m_VAOs[VAO_Cube]);

    int numCubes = graph.length();

    for (int k=0; k<numCubes; ++k)
    {
        Cube* currentCube = graph[k];
        QMatrix4x4 originalModelViewMatrix(modelViewMatrix);
        QMatrix4x4 currentCubeTranformation = currentCube->getTransformation();
        modelViewMatrix*=currentCubeTranformation;

        // Set cube own "lighting" parameters in shader
        m_programRender->setUniformValue(m_cubeAmbient, currentCube->ambient);
        m_programRender->setUniformValue(m_cubeDiffuse, currentCube->diffuse);
        m_programRender->setUniformValue(m_cubeSpecular, currentCube->specular);
        m_programRender->setUniformValue(m_cubeColor, currentCube->color);
        m_programRender->setUniformValue(m_newCube, currentCube->isNewCube);

        // Translate to current cube transformation
        m_programRender->setUniformValue(m_mvMatrixLocation, modelViewMatrix);
        m_programRender->setUniformValue(m_lightMvpMatrixLoc, m_lightViewProjMatrix*modelViewMatrix);

        // Assign textures to all cubes
        m_programRender->setUniformValue(m_texColorLocation, 1);
        m_programRender->setUniformValue(m_texNormalLocation, 2);

        glActiveTexture(GL_TEXTURE1);
        m_textureColor->bind();
        glActiveTexture(GL_TEXTURE2);
        m_textureNormal->bind();

        bool drawSelectedCubeOnClick = k == m_selectedCubeOnClick;
        bool drawSelectedCubeOnHover = k == selectedCubeOnHover;

        m_programRender->setUniformValue(m_drawingSelectedCubeOnClick, drawSelectedCubeOnClick);

        for (int n=0; n<6; n++)
        {
            bool drawSelectedFace = m_selectedFace%6 == n && drawSelectedCubeOnHover;
            m_programRender->setUniformValue(m_drawingSelectedFace, drawSelectedFace);

            // Draw the face with the color of selection
            glDrawRangeElements(GL_TRIANGLES,0,6,(n+1)*6,GL_UNSIGNED_INT,nullptr);
            // Reset draw selected face option
            m_programRender->setUniformValue(m_drawingSelectedFace, false);
        }
        // Restore previous transformations
        modelViewMatrix = originalModelViewMatrix;
        // Reset draw selected cube option
        m_programRender->setUniformValue(m_drawingSelectedCubeOnClick, false);
    }
    drawMesh();
}

void Viewer::drawMesh()
{
    // Draw the mesh
    camera()->setSceneRadius(100);

    m_programRender->setUniformValue(m_mvMatrixLocation, toolTransform);

    for (unsigned int i=0; i<_meshesGL.size(); ++i)
    {
        // Draw the mesh
        glBindVertexArray(_meshesGL[i].vao);
        glDrawArrays(GL_TRIANGLES, 0, _meshesGL[i].numVertices);
    }
}

void Viewer::init()
{
    setMouseTracking(true);
    setMouseBinding(Qt::ShiftModifier, Qt::LeftButton, CAMERA, NO_MOUSE_ACTION);

    // Initialize openGL
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &Viewer::cleanup);
    initializeOpenGLFunctions();

    // Init shaders
    initRenderShaders();
    initPickingShaders();
    initShadowShaders();

    // Create our VertexArrays Objects and VertexBuffer Objects
    glGenVertexArrays(NumVAOs, m_VAOs);
    glGenBuffers(NumBuffers, m_Buffers);
    setSceneRadius(10);
    //camera()->setZNearCoefficient(0.01);
    camera()->showEntireScene();

    initScene();
    initGeometryCube();

    // Init GL properties
    glPointSize(10.0f);

    // Load textures
    m_textureColor = new QOpenGLTexture(QImage("assets/granite_floor.jpg").mirrored());
    m_textureColor->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_textureColor->setMagnificationFilter(QOpenGLTexture::Linear);

    m_textureNormal = new QOpenGLTexture(QImage("assets/granite_floor_normals.jpg").mirrored());
    m_textureNormal->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    m_textureNormal->setMagnificationFilter(QOpenGLTexture::Linear);

    // Load the 3D model from the obj file
    loadObjFile("assets/tournevis.obj");
    toolTransform.setToIdentity();
    toolTransform.translate(QVector3D(3,-3,-11));
}

void Viewer::initShadowShaders()
{
    // Create shadow map shader
    m_shadowMapShader = new QOpenGLShaderProgram;
    if (!m_shadowMapShader->addShaderFromSourceFile(QOpenGLShader::Vertex, "shadow.vert")) {
        cerr << "Unable to load shader" << endl
             << "Log file:" << endl;
        qDebug() << m_shadowMapShader->log();
    }
    if (!m_shadowMapShader->addShaderFromSourceFile(QOpenGLShader::Fragment, "shadow.frag")) {
        cerr << "Unable to load shader" << endl
             << "Log file:" << endl;
        qDebug() << m_shadowMapShader->log();
    }

    m_shadowMapShader->link();
    m_shadowMapShader->bind();

    if((m_mvpMatrixLoc_shadow = m_shadowMapShader->uniformLocation("mvpMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "mvpMatrix";

    if((m_vPositionLoc_shadow = m_shadowMapShader->attributeLocation("vPosition")) < 0)
        qDebug() << "Unable to find shader location for " << "vPosition";

    m_shadowFBO = new QOpenGLFramebufferObject(ShadowSizeX, ShadowSizeY, QOpenGLFramebufferObject::Depth);
    m_shadowFBO->bind();
    m_shadowFBO->release();
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

    if ((m_vUVLocation = m_programRender->attributeLocation("vUV")) < 0)
        qDebug() << "Unable to find shader location for " << "vUV";

    if ((m_vTangentLocation = m_programRender->attributeLocation("vTangent")) < 0)
        qDebug() << "Unable to find shader location for " << "vTangent";

    if ((m_mvMatrixLocation = m_programRender->uniformLocation("mvMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "mvMatrix";

    if ((m_projMatrixLocation = m_programRender->uniformLocation("projMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "projMatrix";

    if ((m_viewMatrixLocation = m_programRender->uniformLocation("viewMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "viewMatrix";

    if ((m_normalMatrixLocation = m_programRender->uniformLocation("normalMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "normalMatrix";

    if ((m_drawingSelectedCubeOnClick = m_programRender->uniformLocation("drawingSelectedCubeOnClick")) < 0)
        qDebug() << "Unable to find shader location for " << "drawingSelectedCubeOnClick";

    if ((m_drawingSelectedFace = m_programRender->uniformLocation("drawingSelectedFace")) < 0)
        qDebug() << "Unable to find shader location for " << "drawingSelectedFace";

    if ((m_cubeSpecular = m_programRender->uniformLocation("cubeSpecular")) < 0)
        qDebug() << "Unable to find shader location for " << "cubeSpecular";

    if ((m_cubeAmbient = m_programRender->uniformLocation("cubeAmbient")) < 0)
        qDebug() << "Unable to find shader location for " << "cubeAmbient";

    if ((m_cubeDiffuse = m_programRender->uniformLocation("cubeDiffuse")) < 0)
        qDebug() << "Unable to find shader location for " << "cubeDiffuse";

    if ((m_cubeColor = m_programRender->uniformLocation("cubeColor")) < 0)
        qDebug() << "Unable to find shader location for " << "cubeColor";

    if ((m_newCube = m_programRender->uniformLocation("newCube")) < 0)
        qDebug() << "Unable to find shader location for " << "newCube";

    if ((m_spotLightPositionLocation = m_programRender->uniformLocation("spotLightPosition")) < 0)
        qDebug() << "Unable to find shader location for " << "spotLightPosition";

    if ((m_spotLightDirectionLocation = m_programRender->uniformLocation("spotLightDirection")) < 0)
        qDebug() << "Unable to find shader location for " << "spotLightDirection";

    if ((m_texColorLocation = m_programRender->uniformLocation("texColor")) < 0)
        qDebug() << "Unable to find shader location for " << "texColor";

    if ((m_texNormalLocation = m_programRender->uniformLocation("texNormal")) < 0)
        qDebug() << "Unable to find shader location for " << "texNormal";

    if ((m_lightMvpMatrixLoc = m_programRender->uniformLocation("mvpLightMatrix")) < 0)
        qDebug() << "Unable to find shader location for " << "mvpLightMatrix";
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
    GLfloat tangents[numVertices][3];
    GLfloat UVs[numVertices][2];
    GLint indices[numIndices];

    // Generate surrounding vertices
    int v = 0;

   QQueue<Cube*> queueCube = graph;
   int sizeQueue = queueCube.length();

   for (int i=0; i<sizeQueue; ++i)
    {
        if (v<numVertices) {

            Cube* currentCube = queueCube[i] ;
            QQueue<QVector3D> cubeVertices = currentCube->getVertices();
            QQueue<QVector2D> cubeUVs = currentCube->getUVs();

            while (!cubeVertices.isEmpty()) {
                QVector3D currentVertice = cubeVertices.dequeue();
                QVector3D currentNormal = currentCube->Normales[v%numVerticePerCube];
                QVector3D currentTangent = currentCube->getTangent(v%numVerticePerCube);
                QVector2D currentUVs = cubeUVs.dequeue();

                vertices[v][0] = currentVertice.x();
                vertices[v][1] = currentVertice.y();
                vertices[v][2] = currentVertice.z();

                normals[v][0] = currentNormal.x();
                normals[v][1] = currentNormal.y();
                normals[v][2] = currentNormal.z();

                tangents[v][0] = currentTangent.x();
                tangents[v][1] = currentTangent.y();
                tangents[v][2] = currentTangent.z();

                UVs[v][0] = currentUVs.x();
                UVs[v][1] = currentUVs.y();

                // qInfo() << "vertices" << v;
                // qInfo() << QString::number(normals[v][0]);
                // qInfo() << QString::number(normals[v][1]);
                // qInfo() << QString::number(normals[v][2]);

                v++;
            }
        }
    }

    for (int i=0; i<numIndices; ++i)
    {
        indices[i] = Cube().getIndice(i%numIndicePerCube)+(i/numIndicePerCube)*numVerticePerCube ;

        //qInfo() << "indec " << i ;
        //qInfo() << QString::number(indices[i]);
    }
    // Fill vertex VBO
    GLsizeiptr offsetVertices = 0;
    GLsizeiptr offsetNormals = sizeof(vertices);
    GLsizeiptr offsetUVs = offsetNormals + GLsizeiptr(sizeof(normals));
    GLsizeiptr offsetTangents = offsetUVs + GLsizeiptr(sizeof(UVs));
    GLsizeiptr dataSize = offsetTangents + GLsizeiptr(sizeof(tangents));

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[VBO_Cube]);
    glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, offsetVertices, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, offsetNormals, sizeof(normals), normals);
    glBufferSubData(GL_ARRAY_BUFFER, offsetUVs, sizeof(UVs), UVs);
    glBufferSubData(GL_ARRAY_BUFFER, offsetTangents, sizeof(tangents), tangents);

    // Set VAO
    glBindVertexArray(m_VAOs[VAO_Cube]);

    glUseProgram(m_programRender->programId());
    glVertexAttribPointer(GLuint(m_vPositionLocation), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetVertices));
    glEnableVertexAttribArray(GLuint(m_vPositionLocation));

    glVertexAttribPointer(GLuint(m_vNormalLocation), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetNormals));
    glEnableVertexAttribArray(GLuint(m_vNormalLocation));

    glVertexAttribPointer(GLuint(m_vUVLocation), 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetUVs));
    glEnableVertexAttribArray(GLuint(m_vUVLocation));

    glVertexAttribPointer(GLuint(m_vTangentLocation), 3, GL_FLOAT, GL_TRUE, 0, BUFFER_OFFSET(offsetTangents));
    glEnableVertexAttribArray(GLuint(m_vTangentLocation));

    // Fill in indices EBO
    // Note: The current VAO will remember the call to glBindBuffer for a GL_ELEMENT_ARRAY_BUFFER.
    // However, we will need to call glDrawElements() instead of glDrawArrays().
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[EBO_Cube]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glUseProgram(m_programPicking->programId());
    glVertexAttribPointer(GLuint(m_vPositionLocationPicking), 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetVertices));
    glEnableVertexAttribArray(GLuint(m_vPositionLocationPicking));

    glUseProgram(m_shadowMapShader->programId());
    glVertexAttribPointer(m_vPositionLocation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offsetVertices));
    glEnableVertexAttribArray(GLuint(m_vPositionLoc_shadow));

    glClearColor(0.5f, 0.5f, 0.5f, 1.0);// add background
}

void Viewer::initScene()
{
    const float dimArret = Cube::dimArret;
    for (int i=0; i < numCubesPerRow; ++i)
    {
        for (int j=0; j < numCubesPerCol; ++j)
        {
            Cube* currentCube = new Cube();
            QMatrix4x4 cubeTranformation;

            // Center the scene with QVector3D(numCubesPerRow-1, 0, numCubesPerCol-1) * dimArret/2
            cubeTranformation.translate(QVector3D(i*dimArret, 0, j*dimArret) - QVector3D(numCubesPerRow-1, 0, numCubesPerCol-1) * dimArret/2);
            currentCube->addTransformation(cubeTranformation);
            graph.append(currentCube);
        }
    }
}

void Viewer::addCube()
{
    if(m_selectedFace < 0 || animationIsStarted()) return;

    toolManipulation();

    Cube* newCube = new Cube();

    QVector3D color = QVector3D(2.0*getCubeR(), 2.0*getCubeG(), 2.0*getCubeB());
    newCube->setColor(color);
    newCube->setIsNewCube(true);

    QMatrix4x4 newCubeTranformation;
    Cube* currentCubeSelected = graph[selectedCubeOnHover];
    QVector3D normal = newCube->getNormal(m_selectedFace);
    normal.normalize();
    newCubeTranformation.translate(normal * Cube::dimArret);

    currentCubeSelected->addChild(newCube);

    newCube->addTransformation(currentCubeSelected->getTransformation()*newCubeTranformation);
    graph.append(newCube);
}

void Viewer::deleteCube()
{
    if(selectedCubeOnHover < 0) return;

    Cube* currentCube = graph[selectedCubeOnHover];

    int numCubes = graph.length();
    for(int k = 0; k < numCubes; k++){
        Cube* parentCube = graph[k];
        parentCube->removeChild(currentCube);
    }

    graph.removeOne(currentCube);

}

void Viewer::mousePressEvent(QMouseEvent *e)
{
    QGLViewer::mousePressEvent(e);
    std::cout << "Viewer::mousePressEvent" << std::endl;

    if ((e->button() == Qt::LeftButton))
    {
        if((e->modifiers() == Qt::ShiftModifier))
        {
            addCube();
            update();

        }
        if((e->modifiers() == Qt::CTRL))
        {
            performSelection(e->x(), e->y(), true);
        }
        if(e->modifiers() == Qt::CTRL+Qt::ShiftModifier)
        {
            scaleCube();
        }
    }
    if((e->button() == Qt::RightButton))
    {
            deleteCube();
            update();
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *e)
{
    QGLViewer::mouseMoveEvent(e);
    performSelection(e->x(), e->y(), false);

    update();
}

void Viewer::shadowRender()
{
    // Render the scene from the light's point of view.
    // Setup the offscreen frame buffer we'll use to store the depth image.
    m_shadowFBO->bind();

    glClearColor(1, 1, 1, 1);
    // Enable depth test.
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute the light projection matrix.
    GLfloat lightFOV = 50.f;
    QMatrix4x4 lightProjMatrix;
    lightProjMatrix.perspective(lightFOV, ShadowSizeX/ShadowSizeY, 0.01f, 100.0f);

    // Compute the light view matrix.
    QVector3D at(0.f,0.f,0.f);
    QVector3D up(0.f,1.f,0.f);
    QVector3D lightPos(m_spotLightPosition.x(), m_spotLightPosition.y(), m_spotLightPosition.z());
    QMatrix4x4 lightViewMatrix;
    lightViewMatrix.lookAt(lightPos, at, up);
    m_lightViewProjMatrix = lightProjMatrix * lightViewMatrix;

    // Bind the shadow shader program.
    m_shadowMapShader->bind();

    glBindVertexArray(m_VAOs[VAO_Cube]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[EBO_Cube]);

    int numCubes = graph.length();

    for (int k=0; k<numCubes; ++k)
    {
        Cube* currentCube = graph[k];
        QMatrix4x4 currentCubeTranformation = currentCube->getTransformation();
        QMatrix4x4 modelViewMatrix;
        modelViewMatrix.setToIdentity();
        modelViewMatrix*=currentCubeTranformation;

        // Translate to current cube transformation
        m_shadowMapShader->setUniformValue(m_mvpMatrixLoc_shadow, m_lightViewProjMatrix*modelViewMatrix);

        for (int n=0; n<6; n++)
        {
            // Draw the face with the color of selection
            glDrawRangeElements(GL_TRIANGLES,0,6,(n+1)*6,GL_UNSIGNED_INT,nullptr);
        }
    }
    //Finish drawing and release the framebuffer.
    glFinish();
    m_shadowFBO->release();
}

void Viewer::performSelection(int x, int y, bool selectCubeOnClick)
{
    // Map (dictionnary) used to store the correspondences between colors and faces.
    QMap<unsigned int, int> myMap;
    // Identificator ID used as a color for rendering and as a key for the map.
    QRgb id = 0;

    makeCurrent();	// This allows us to use OpenGL functions outside of Viewer::draw()

    std::cout << "Viewer::performSelection(" << x << ", " << y << ")" << std::endl;

    glEnable(GL_DEPTH_TEST);
    // Selection is performed by drawing the cube faces with a color that matches their ID
    // Note: Because we are drawing outside the draw() function, the back buffer is not
    //       swapped after this function is called.

    // Clear back buffer. Since we set a different clear color, we save the
    // previous value and restore it after the backbuffer has been cleared.
    float clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);

    glClearColor(1,1,1,1);	// This value should not match any existing index
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    // Bind our vertex/fragment shaders
    m_programPicking->bind();

    // Get projection and camera transformations
    QMatrix4x4 projectionMatrix;
    QMatrix4x4 modelViewMatrix;
    camera()->getProjectionMatrix(projectionMatrix);
    camera()->getModelViewMatrix(modelViewMatrix);

    // Draw the cubes
    m_programPicking->setUniformValue(m_projMatrixLocationPicking, projectionMatrix);

    int numCubes = graph.length();
    glBindVertexArray(m_VAOs[VAO_Cube]);
    for (int k=0; k<numCubes; ++k)
    {
        // Save transformations
        QMatrix4x4 originalModelViewMatrix(modelViewMatrix);
        QMatrix4x4 currentCubeTranformation = graph[k]->getTransformation();

        // Translate to current cube transformation
        m_programPicking->setUniformValue(m_mvMatrixLocationPicking, modelViewMatrix*currentCubeTranformation);

        // For convenience, convert the ID to a color obj-ect.
        for (int n=0; n<6; n++)
        {
            QColor color;
            color.setRgba(id);
            // Get the equivalent of the color as an unsigned long.
            unsigned int key = color.rgba();
            // Insert the key (unsigned long) in the map.
            myMap.insert(key, id);
            // Set the color value for the shader.
            m_programPicking->setUniformValue(m_colorLocationPicking, color);

            // Draw the face
            glDrawRangeElements(GL_TRIANGLES,0,6,(n+1)*6,GL_UNSIGNED_INT,nullptr);

            // Increment the ID, as it is not in use.
            id++;
        }
        // Restore previous transformations
        modelViewMatrix = originalModelViewMatrix;
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

    m_selectedFace = value;
    std::cout << "m_selectedFace: " << m_selectedFace << std::endl;

    selectedCubeOnHover = floor(value/6.f);
    std::cout << "m_selectedCubeOnHover: " << selectedCubeOnHover << std::endl;

    if(selectCubeOnClick && !animationIsStarted())
    {
        m_selectedCubeOnClick = selectedCubeOnHover;
        std::cout << "m_selectedCubeOnClick: " << m_selectedCubeOnClick << std::endl;
    }

    // We are done with OpenGL
    doneCurrent();
}

void Viewer::rotateAroundXAxisPositive() {
    rotateAroundAxis(QVector3D(1.f,0.f,0.f));
}

void Viewer::rotateAroundXAxisNegative() {
    rotateAroundAxis(QVector3D(-1.f,0.f,0.f));
}

void Viewer::rotateAroundYAxisPositive() {
    rotateAroundAxis(QVector3D(0.f,1.f,0.f));
}

void Viewer::rotateAroundYAxisNegative() {
    rotateAroundAxis(QVector3D(0.f,-1.f,0.f));
}

void Viewer::rotateAroundZAxisPositive() {
    rotateAroundAxis(QVector3D(0.f,0.f,1.f));
}

void Viewer::rotateAroundZAxisNegative() {
    rotateAroundAxis(QVector3D(0.f,0.f,-1.f));
}

void Viewer::rotateAroundAxis(QVector3D axis) {

    if(animationIsStarted() || m_selectedCubeOnClick < 0) return;

    Cube* currentCubeSelected = graph[m_selectedCubeOnClick];

    currentAnimation = animationType::rotation;
    animationAxis = axis;
    backToPlanOriginM = currentCubeSelected->getTransformation().inverted();
    backToCubeOriginM = currentCubeSelected->getTransformation();
    startAnimation();
}
void Viewer::scaleCube()
{
    if(animationIsStarted() || m_selectedCubeOnClick < 0) return;

    Cube* currentCubeSelected = graph[m_selectedCubeOnClick];

    currentAnimation = animationType::scaling;
    backToPlanOriginM = currentCubeSelected->getTransformation().inverted();
    backToCubeOriginM = currentCubeSelected->getTransformation();
    startAnimation();
}

void Viewer::toolManipulation()
{
    if(animationIsStarted()) return;

    currentAnimation = animationType::toolRotation;
    animationAxis = QVector3D(-1.f,0.f,0.f);
    startAnimation();
}
void Viewer::animate() {


    Cube* currentCubeSelected;
    QMatrix4x4 transformation;
    switch(currentAnimation)
    {
        case animationType::rotation:

            if(m_selectedCubeOnClick < 0) return;
            currentCubeSelected = graph[m_selectedCubeOnClick];

            transformation.rotate(animationIterationAngle, animationAxis);

            animationCurrentAngle+=animationIterationAngle;

            currentCubeSelected->addTransformation(backToCubeOriginM * transformation * backToPlanOriginM);

            if(animationCurrentAngle >= animationMaxAngle)
            {
                stopAnimation();
                animationCurrentAngle = 0;
            }
            break;

        case animationType::toolRotation:

            if(!meshRotationReachedEnd)
            {
                toolTransform.rotate(animationIterationAngle, animationAxis);
                animationCurrentAngle+=animationIterationAngle;
            }
            else
            {
                toolTransform.rotate(-animationIterationAngle, animationAxis);
                animationCurrentAngle-=animationIterationAngle;
            }
            if(animationCurrentAngle >= animationMaxAngle) meshRotationReachedEnd = true;

            if(meshRotationReachedEnd && animationCurrentAngle <= 0)
            {
                stopAnimation();
                animationCurrentAngle = 0;
                meshRotationReachedEnd = false;
            }
            break;

        case animationType::scaling:

            if(m_selectedCubeOnClick < 0) return;

            currentCubeSelected = graph[m_selectedCubeOnClick];

            float scale = 1.1;

            if (animationCurrentAngle> 10)
                scale = 0.9;

            transformation.scale(scale,scale,scale);
            currentCubeSelected->addTransformation(backToCubeOriginM * transformation * backToPlanOriginM);
            animationCurrentAngle++;

            if(animationCurrentAngle >= 20)
            {
                stopAnimation();
                transformation.scale(1,1,1);
                currentCubeSelected->addTransformation(backToCubeOriginM * transformation * backToPlanOriginM);
                animationCurrentAngle = 0;
            }
            break;
    }
}

void Viewer::loadObjFile(const std::string filePath)
{
    // Load the obj file
    OBJLoader::Loader loader(filePath);

    // Create a GL object for each mesh extracted from the OBJ file
    const std::vector<OBJLoader::Mesh>& meshes = loader.getMeshes();
    const std::vector<OBJLoader::Material>& materials = loader.getMaterials();
    for (unsigned int i=0; i<meshes.size(); ++i)
    {
        if (meshes[i].vertices.size()==0)
            continue;

        MeshGL meshGL;
        meshGL.numVertices = meshes[i].vertices.size();

        // Set material properties of the mesh
        const float *Kd = materials[meshes[i].materialID].Kd;
        const float *Ks = materials[meshes[i].materialID].Ks;

        meshGL.diffuse = QVector3D(Kd[0], Kd[1], Kd[2]);
        meshGL.specular = QVector3D(Ks[0], Ks[1], Ks[2]);
        meshGL.specularExponent = materials[meshes[i].materialID].Kn;

        // Create its VAO and VBO object
        glGenVertexArrays(1, &meshGL.vao);
        glGenBuffers(1, &meshGL.vbo);

        // Fill VBO with vertices data
        GLsizei dataSize = meshes[i].vertices.size() * sizeof(OBJLoader::Vertex);
        GLsizei stride = sizeof(OBJLoader::Vertex);
        GLsizeiptr positionOffset = 0;
        GLsizeiptr normalOffset = sizeof(OBJLoader::Vertex::position);

        glBindBuffer(GL_ARRAY_BUFFER, meshGL.vbo);
        glBufferData(GL_ARRAY_BUFFER, dataSize, &meshes[i].vertices[0], GL_STATIC_DRAW);

        // Set VAO that binds the shader vertices inputs to the buffer data
        glBindVertexArray(meshGL.vao);

        glVertexAttribPointer(m_vPositionLocation, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(positionOffset));
        glEnableVertexAttribArray(m_vPositionLocation);

        glVertexAttribPointer(m_vNormalLocation, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(normalOffset));
        glEnableVertexAttribArray(m_vNormalLocation);

        // Add it to the list
        _meshesGL.push_back(meshGL);
    }
}

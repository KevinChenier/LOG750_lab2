#include "simpleViewer.h"

#include <QOpenGLShaderProgram>
#include <QMouseEvent>

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
    delete m_programRender;
    m_programRender = nullptr;

    // Delete buffers
    glDeleteBuffers(NumBuffers, m_Buffers);
    glDeleteVertexArrays(NumVAOs, m_VAOs);

    // outil
    for (unsigned int i=0; i<_meshesGL.size(); ++i)
        {
            // Set material properties

            // Draw the mesh
            glDeleteVertexArrays(1, &_meshesGL[i].vao);
            glDeleteBuffers(1, &_meshesGL[i].vbo);
        }
    _meshesGL.clear();
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

     initGeometryCube();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBindVertexArray(m_VAOs[VAO_Cube]);

    int numCubes = graph.length();

    for (int k=0; k<numCubes; ++k)
    {
        Cube* currentCube = graph[k];
        QMatrix4x4 originalModelViewMatrix(modelViewMatrix);
        QMatrix4x4 currentCubeTranformation = currentCube->getTransformation();

        // Set cube "material" in shader
        m_programRender->setUniformValue(m_cubeAmbiant, currentCube->ambiant);
        m_programRender->setUniformValue(m_cubeDiffuse, currentCube->diffuse);
        m_programRender->setUniformValue(m_cubeSpecular, currentCube->specular);

        m_programRender->setUniformValue(m_cubeColor, currentCube->color);
        m_programRender->setUniformValue(m_newCube, currentCube->isNewCube);

        // Translate to current cube transformation
        m_programRender->setUniformValue(m_mvMatrixLocation, modelViewMatrix*currentCubeTranformation);

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
    // draw outil
    // Draw the meshes

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

    // Create our VertexArrays Objects and VertexBuffer Objects
    glGenVertexArrays(NumVAOs, m_VAOs);
    glGenBuffers(NumBuffers, m_Buffers);
    setSceneRadius(10);
    camera()->showEntireScene();

    initScene();
    initGeometryCube();

    // Init GL properties
    glPointSize(10.0f);

    // Load the 3D model from the obj file
    loadObjFile("assets/tournevis.obj");
    toolTransform.setToIdentity();
    toolTransform.translate(QVector3D(3,-3,-15));
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

    if ((m_drawingSelectedCubeOnClick = m_programRender->uniformLocation("drawingSelectedCubeOnClick")) < 0)
        qDebug() << "Unable to find shader location for " << "drawingSelectedCubeOnClick";

    if ((m_drawingSelectedFace = m_programRender->uniformLocation("drawingSelectedFace")) < 0)
        qDebug() << "Unable to find shader location for " << "drawingSelectedFace";

    if ((m_cubeSpecular = m_programRender->uniformLocation("cubeSpecular")) < 0)
        qDebug() << "Unable to find shader location for " << "cubeSpecular";

    if ((m_cubeAmbiant = m_programRender->uniformLocation("cubeAmbiant")) < 0)
        qDebug() << "Unable to find shader location for " << "cubeAmbiant";

    if ((m_cubeDiffuse = m_programRender->uniformLocation("cubeDiffuse")) < 0)
        qDebug() << "Unable to find shader location for " << "cubeDiffuse";

    if ((m_cubeColor = m_programRender->uniformLocation("cubeColor")) < 0)
        qDebug() << "Unable to find shader location for " << "cubeColor";

    if ((m_newCube = m_programRender->uniformLocation("newCube")) < 0)
        qDebug() << "Unable to find shader location for " << "newCube";

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

   QQueue<Cube*> queueCube = graph;
   int sizeQueue = queueCube.length();

   for (int i=0; i<sizeQueue; ++i)
    {
        if (v<numVertices) {

            Cube* currentCube = queueCube[i] ;
            QMatrix4x4 currentCubeTransformation = currentCube->getTransformation();
            QQueue<QVector3D> cubeVertices = currentCube->getVertices() ;

            while (!cubeVertices.isEmpty()) {
                QVector3D currentVertice = cubeVertices.dequeue();
                QVector3D currentNormal =  currentCubeTransformation*currentCube->Normales[v%numVerticePerCube] ;

                vertices[v][0] = currentVertice.x();
                vertices[v][1] = currentVertice.y();
                vertices[v][2] = currentVertice.z();

                normals[v][0] = currentNormal.x();
                normals[v][1] = currentNormal.y();
                normals[v][2] = currentNormal.z();

//                qInfo() << "vertices" << v;
//                qInfo() << QString::number(normals[v][0]);
//                qInfo() << QString::number(normals[v][1]);
//                qInfo() << QString::number(normals[v][2]);

                v++;
            }
        }
    }

    for (int i=0; i<numIndices; ++i)
    {
        indices[i] = Cube().indices[i%numIndicePerCube]+(i/numIndicePerCube)*numVerticePerCube ;

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
            Cube* currentCube = new Cube();
            QMatrix4x4 cubeTranformation;

            int k = i+j;
            currentCube->ambiant.setX(k%2);
            currentCube->ambiant.setY(k%4);
            currentCube->ambiant.setZ(k%6);

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

    Cube* newCube = new Cube();

    QVector3D color = QVector3D(2.0*getCubeR(), 2.0*getCubeG(), 2.0*getCubeB());
    newCube->setColor(color);
    newCube->setIsNewCube(true);

    QMatrix4x4 newCubeTranformation;
    Cube* currentCubeSelected = graph[selectedCubeOnHover];
    QMatrix4x4 modelViewMatrix;
    camera()->getModelViewMatrix(modelViewMatrix);
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
    initGeometryCube();
    QGLViewer::mousePressEvent(e);
    std::cout << "Viewer::mousePressEvent" << std::endl;

    if ((e->button() == Qt::LeftButton))
    {
        if((e->modifiers() == Qt::ShiftModifier))
        {
            addCube();
            toolManipulation();
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
        //GLsizeiptr uvOffset = normalOffset + sizeof(OBJLoader::Vertex::normal);

        glBindBuffer(GL_ARRAY_BUFFER, meshGL.vbo);
        glBufferData(GL_ARRAY_BUFFER, dataSize, &meshes[i].vertices[0], GL_STATIC_DRAW);

//        // Set VAO that binds the shader vertices inputs to the buffer data
        glBindVertexArray(meshGL.vao);

        glVertexAttribPointer(m_vPositionLocation, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(positionOffset));
        glEnableVertexAttribArray(m_vPositionLocation);

        glVertexAttribPointer(m_vNormalLocation, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(normalOffset));
        glEnableVertexAttribArray(m_vNormalLocation);

        // Add it to the list
        _meshesGL.push_back(meshGL);
    }
}

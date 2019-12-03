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

#ifndef SIMPLEVIEWER_H
#define SIMPLEVIEWER_H

#include <QOpenGLFunctions_4_0_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QMatrix4x4>
#include <QVector3D>
#include <QGLViewer/qglviewer.h>
#include <node.h>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLFramebufferObject)

class Viewer : public QGLViewer, protected QOpenGLFunctions_4_0_Core
{
    Q_OBJECT
public:
    Viewer();
    ~Viewer();

    GLfloat getCubeR() const { return cubeR; }
    GLfloat getCubeG() const { return cubeG; }
    GLfloat getCubeB() const { return cubeB; }

public slots:
    void cleanup();

    void rotateAroundXAxisPositive();
    void rotateAroundXAxisNegative();
    void rotateAroundYAxisPositive();
    void rotateAroundYAxisNegative();
    void rotateAroundZAxisPositive();
    void rotateAroundZAxisNegative();
    void rotateAroundAxis(QVector3D axis);

    void setCubeR(double r) { cubeR = r; }
    void setCubeG(double g) { cubeG = g; }
    void setCubeB(double b) { cubeB = b; }

protected :
    virtual void draw();
    virtual void init();

    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void animate();

private:

    void loadObjFile(const std::string filePath);

    void initRenderShaders();
    void initPickingShaders();
    void initShadowShaders();
    void initGeometryCube();
    void initScene();
    void toolManipulation();

    void performSelection(int x, int y, bool selectCubeOnClick);
    void shadowRender();

    void addCube();
    void scaleCube();
    void deleteCube();

    void drawMesh();

    // VAOs and VBOs
    struct MeshGL
    {
      GLuint vao;
      GLuint vbo;

      QVector3D diffuse;
      QVector3D specular;
      GLfloat specularExponent;

      unsigned int numVertices;
    };

    enum animationType { rotation, scaling ,toolRotation};

    // VAOs and VBOs
    enum VAO_IDs { VAO_Cube, NumVAOs };
    enum Buffer_IDs { VBO_Cube, EBO_Cube, NumBuffers };

    GLuint m_VAOs[NumVAOs];
    GLuint m_Buffers[NumBuffers];

    animationType currentAnimation = animationType::rotation;

    QVector4D m_spotLightPosition = QVector4D(0.f, 1.f, 0.f, 1.f);
    QVector4D m_spotLightDirection = QVector3D(0.f, -1.f, 0.f);
    QMatrix4x4 m_lightViewProjMatrix;

    // Render shaders & locations
    QOpenGLShaderProgram *m_programRender;
    int m_vPositionLocation, m_vNormalLocation, m_vUVLocation, m_vTangentLocation;
    int m_drawingSelectedCubeOnClick, m_drawingSelectedFace;
    int m_spotLightPositionLocation, m_spotLightDirectionLocation, m_lightMvpMatrixLoc;
    int m_projMatrixLocation, m_mvMatrixLocation, m_normalMatrixLocation;
    int m_vPositionTool, m_vNormalTool;
    int m_texColorLocation, m_texNormalLocation;

    // Picking shaders & locations
    QOpenGLShaderProgram *m_programPicking;
    int m_vPositionLocationPicking;
    int m_colorLocationPicking;
    int m_projMatrixLocationPicking, m_mvMatrixLocationPicking;

    // Shadow shaders & locations
    QOpenGLShaderProgram *m_shadowMapShader;
    QOpenGLFramebufferObject *m_shadowFBO;
    int m_vPositionLoc_shadow, m_mvpMatrixLoc_shadow;

    // Picking
    int m_selectedFace, m_selectedCubeOnClick, selectedCubeOnHover;

    // Cubes
    int m_cubeAmbient, m_cubeSpecular, m_cubeDiffuse;

    // New cube color chosen
    int m_cubeColor;
    int m_newCube;

    // Cube textures
    QOpenGLTexture *m_textureColor, *m_textureNormal;

    // Colour of Cube
    GLfloat cubeR = 0.0f;
    GLfloat cubeG = 1.0f;
    GLfloat cubeB = 0.0f;

    GLboolean isNewCube = false;

    QVector3D animationAxis;
    float animationCurrentAngle;
    QMatrix4x4 backToPlanOriginM, backToCubeOriginM;

    bool meshRotationReachedEnd = false;

    static constexpr float animationMaxAngle = 90.f;
    static constexpr float animationIterationAngle = animationMaxAngle/3.f;

    std::vector<MeshGL> _meshesGL;
};

#endif // SIMPLEVIEWER_H

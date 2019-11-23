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

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

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
    enum animationType { rotation, scaling ,toolRotation};

    animationType currentAnimation = animationType::rotation;

    void initRenderShaders();
    void initPickingShaders();
    void initDLightingShaders();
    void initGeometryCube();
    void initScene();
    void toolManipulation();

    void performSelection(int x, int y, bool selectCubeOnClick);

    void addCube();
    void scaleCube();
    void deleteCube();

    // VAOs and VBOs
    enum VAO_IDs { VAO_Cube, NumVAOs };
    enum Buffer_IDs { VBO_Cube, EBO_Cube, NumBuffers };

    GLuint m_VAOs[NumVAOs];
    GLuint m_Buffers[NumBuffers];

    // Render shaders & locations
    QOpenGLShaderProgram *m_programRender;
    int m_vPositionLocation;
    int m_drawingSelectedCubeOnClick;
    int m_drawingSelectedFace;
    int m_vNormalLocation;
    int m_projMatrixLocation;
    int m_mvMatrixLocation;
    int m_normalMatrixLocation;
    int m_vPositionTool;
    int m_vNormalTool;

    // Picking shaders & locations
    QOpenGLShaderProgram *m_programPicking;
    int m_vPositionLocationPicking;
    int m_colorLocationPicking;
    int m_projMatrixLocationPicking;
    int m_mvMatrixLocationPicking;

    // Picking
    int m_selectedFace;
    int m_selectedCubeOnClick;
    int selectedCubeOnHover;

    // Cubes
    int m_cubeAmbiant;
    int m_cubeSpecular;
    int m_cubeDiffuse;

    int m_cubeColor;
    int m_newCube;

    // Colour of Cube
    GLfloat cubeR = 0.0f;
    GLfloat cubeG = 1.0f;
    GLfloat cubeB = 0.0f;

    GLboolean isNewCube = false;

    QVector3D animationAxis;
    float animationCurrentAngle;
    QMatrix4x4 backToPlanOriginM;
    QMatrix4x4 backToCubeOriginM;

    static constexpr float animationMaxAngle = 90.f;
    static constexpr float animationIterationAngle = animationMaxAngle/3.f;

    // VAOs and VBOs
    struct MeshGL
    {
      GLuint vao;
      GLuint vbo;

      QVector3D diffuse;
      QVector3D specular;
      GLfloat   specularExponent;

      unsigned int numVertices;
    };

    std::vector<MeshGL> _meshesGL;

    void loadObjFile(const std::string filePath);

};

#endif // SIMPLEVIEWER_H

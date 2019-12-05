#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core gui xml opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = log750_lab2
TEMPLATE = app


SOURCES += QGLViewer/camera.cpp \
           OBJLoader.cpp \
           QGLViewer/constraint.cpp \
           QGLViewer/frame.cpp \
           QGLViewer/keyFrameInterpolator.cpp \
           QGLViewer/manipulatedCameraFrame.cpp \
           QGLViewer/manipulatedFrame.cpp \
           QGLViewer/mouseGrabber.cpp \
           QGLViewer/qglviewer.cpp \
           QGLViewer/quaternion.cpp \
           QGLViewer/saveSnapshot.cpp \
           QGLViewer/vec.cpp \
           cube.cpp \
           main.cpp \
           mainwindow.cpp \
           node.cpp \
           simpleViewer.cpp

HEADERS  += QGLViewer/camera.h \
            OBJLoader.h \
            QGLViewer/config.h \
            QGLViewer/constraint.h \
            QGLViewer/domUtils.h \
            QGLViewer/frame.h \
            QGLViewer/keyFrameInterpolator.h \
            QGLViewer/manipulatedCameraFrame.h \
            QGLViewer/manipulatedFrame.h \
            QGLViewer/mouseGrabber.h \
            QGLViewer/qglviewer.h \
            QGLViewer/quaternion.h \
            QGLViewer/vec.h \
            cube.h \
            mainwindow.h \
            node.h \
            simpleViewer.h

DISTFILES += \
    assets/README.txt \
    assets/Thumbs.db \
    assets/assets/README.txt \
    assets/assets/Thumbs.db \
    assets/assets/dry_ground.jpg \
    assets/assets/granite_floor.jpg \
    assets/assets/grass2.jpg \
    assets/assets/limestone_wall.jpg \
    assets/assets/pierre_bouchardee.jpg \
    assets/assets/wood_floor.jpg \
    assets/dry_ground.jpg \
    assets/dry_ground_normals.jpg \
    assets/granite_floor.jpg \
    assets/granite_floor_normals.jpg \
    assets/grass2.jpg \
    assets/grass_normals.jpg \
    assets/limestone_wall.jpg \
    assets/limestone_wall_normals.jpg \
    assets/pierre_bouchardee.jpg \
    assets/pierre_bouchardee_normals.jpg \
    assets/tournevis.mtl \
    assets/tournevis.obj \
    assets/wood_floor.jpg \
    assets/wood_floor_normals.jpg \
    basicShader.frag \
    basicShader.vert \
    constantColor.frag \
    constantColor.vert

FORMS += QGLViewer/ImageInterface.ui mainwindow.ui


CONFIG *= debug_and_release console qt opengl warn_on thread create_prl rtti
CONFIG *= C++11

DEFINES *= QGLVIEWER_STATIC
win32 {
    DEFINES *= NOMINMAX
}

win32 {
  contains ( QT_MAJOR_VERSION, 5 ) {
    greaterThan( QT_MINOR_VERSION, 4) {
      LIBS *= -lopengl32
    }
  }
}

unix:!macx: LIBS += -L$$PWD/irrKlang/bin/linux-gcc-64/ -lIrrKlang

INCLUDEPATH += $$PWD/irrKlang/include
DEPENDPATH += $$PWD/irrKlang/include

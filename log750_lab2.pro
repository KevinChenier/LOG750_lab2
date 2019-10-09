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
           simpleViewer.cpp

HEADERS  += QGLViewer/camera.h \
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
            mainwindow.h \
            node.h \
            simpleViewer.h \
            transform.h

DISTFILES += \
    basicShader.frag \
    basicShader.vert

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

#include "simpleViewer.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set the core profile and version of OpenGL shaders
    QSurfaceFormat fmt;
        fmt.setVersion(4, 0);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(fmt);

    MainWindow w;

    // Instantiate and layout the viewer.
    Viewer *v = new Viewer();
    w.addViewer(v);

    w.show();

    return a.exec();
}

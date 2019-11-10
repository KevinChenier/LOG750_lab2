#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBoxLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addViewer(Viewer* viewer)
{
    QLayout *layout = new QHBoxLayout;
    layout->addWidget(viewer);
    ui->frame->setLayout(layout);
    // rotate X
    QObject::connect(ui->pushButton_2, SIGNAL(clicked(bool)),viewer, SLOT(plusX()));
    QObject::connect(ui->pushButton, SIGNAL(clicked(bool)),viewer, SLOT(negativeX(bool)));
    // rotate Y
    QObject::connect(ui->pushButton_4, SIGNAL(clicked(bool)),viewer, SLOT(plusY(bool)));
    QObject::connect(ui->pushButton_3, SIGNAL(clicked(bool)),viewer, SLOT(negativeY(bool)));
    // rotate Z
    QObject::connect(ui->pushButton_6, SIGNAL(clicked(bool)),viewer, SLOT(plusZ(bool)));
    QObject::connect(ui->pushButton_5, SIGNAL(clicked(bool)),viewer, SLOT(negativeZ(bool)));

}

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
    QObject::connect(ui->pushButton_2, SIGNAL(clicked(bool)),viewer, SLOT(rotateAroundXAxisPositive()));
    QObject::connect(ui->pushButton, SIGNAL(clicked(bool)),viewer, SLOT(rotateAroundXAxisNegative()));
    // rotate Y
    QObject::connect(ui->pushButton_4, SIGNAL(clicked(bool)),viewer, SLOT(rotateAroundYAxisPositive()));
    QObject::connect(ui->pushButton_3, SIGNAL(clicked(bool)),viewer, SLOT(rotateAroundYAxisNegative()));
    // rotate Z
    QObject::connect(ui->pushButton_6, SIGNAL(clicked(bool)),viewer, SLOT(rotateAroundZAxisPositive()));
    QObject::connect(ui->pushButton_5, SIGNAL(clicked(bool)),viewer, SLOT(rotateAroundZAxisNegative()));
    // Texture
    QObject::connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), viewer, SLOT(setTexture(int)));

}

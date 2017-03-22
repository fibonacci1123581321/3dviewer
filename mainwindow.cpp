#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //glw = new OGLWidget;
    //this->setCentralWidget(glw);
    //glw->setFixedSize(500, 500);

    //this->adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
    //delete glw;
}

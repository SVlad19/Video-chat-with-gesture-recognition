#include "mainwindow.h"
#include "servermanager.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ServerManag.reset(new ServerManager());
}

MainWindow::~MainWindow()
{
    delete ui;
}

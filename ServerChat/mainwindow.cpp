#include "mainwindow.h"
#include "servermanager.h"
#include "ui_mainwindow.h"
#include <QTcpSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ServerManag.reset(new ServerManager());
    connect(ServerManag.data(),&ServerManager::NewClient,this,&MainWindow::NewClient);
    connect(ServerManag.data(),&ServerManager::ClientDisconnected,this,&MainWindow::ClientDisconnected);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::NewClient(QTcpSocket *ClientSocket)
{
    if(!ClientSocket){return;}

    int Id = ClientSocket->property("Id").toInt();
    ui->listClients->addItem(QString("New Client added: %1").arg(Id));
}

void MainWindow::ClientDisconnected(QTcpSocket *ClientSocket)
{
    if(!ClientSocket){return;}

    QString ClientName = ClientSocket->property("ClientName").toString();
    ui->listClients->addItem(QString("Client " + ClientName + " disconnected!"));
}



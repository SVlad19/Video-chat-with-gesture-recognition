#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectionwidget.h"
#include "clientmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ConnectionWindowWidget.reset(new ConnectionWidget());
    connect(ConnectionWindowWidget.data(),&ConnectionWidget::ConnectionDataReady,this,&MainWindow::HandleConnectionData);

    Client.reset(new ClientManager(nullptr,this));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionConnect_triggered()
{
    if(ConnectionWindowWidget){
        ConnectionWindowWidget->show();
    }
}

void MainWindow::HandleConnectionData(const QString &IP)
{
    if(Client){
         Client->ConnectToServer(IP);
    }
}

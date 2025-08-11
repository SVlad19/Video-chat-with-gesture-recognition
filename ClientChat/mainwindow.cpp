#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectionwidget.h"
#include "clientmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionDisconnect->setEnabled(false);
    ui->lineClientName->setText("Client");
    ui->centralwidget->setEnabled(false);

    ConnectionWindowWidget.reset(new ConnectionWidget());
    connect(ConnectionWindowWidget.data(),&ConnectionWidget::ConnectionDataReady,this,&MainWindow::HandleConnectionData);

    SetupClient();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupClient()
{
    Client.reset(new ClientManager(nullptr,this));

    connect(Client.data(), &ClientManager::Connected, [this](){
        ui->centralwidget->setEnabled(true);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    });

    connect(Client.data(), &ClientManager::Disconnected, [this](){
        ui->centralwidget->setEnabled(false);
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
    });

}

void MainWindow::HandleConnectionData(const QString &IP)
{
    if(Client){
        Client->ConnectToServer(IP);
    }
}

void MainWindow::on_actionConnect_triggered()
{
    if(ConnectionWindowWidget){
        ConnectionWindowWidget->show();
    }
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionDisconnect_triggered()
{
    if(Client){
        Client->DisconnectFromServer();
    }
}


void MainWindow::on_lineClientName_editingFinished()
{
    if(Client){
        QString Name = ui->lineClientName->text().trimmed();
        Client->SendNewName(Name);
    }
}


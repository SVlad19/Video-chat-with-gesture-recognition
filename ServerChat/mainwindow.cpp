#include "clientchatwidget.h"
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

    ClientChatWidget* ChatWidget = new ClientChatWidget(ClientSocket,this);
    ui->twChats->addTab(ChatWidget,QString("Client (%1").arg(Id));

    connect(ChatWidget, &ClientChatWidget::ClientNameChanged,this,&MainWindow::SetClientName);
    connect(ChatWidget, &ClientChatWidget::StatusChanged,this,&MainWindow::SetClientStatus);
    connect(ChatWidget,&ClientChatWidget::IsTyping,[this](QString Name){
        statusBar()->showMessage(Name,750);
    });
}

void MainWindow::SetClientName(const QString& PrevName, const QString &Name)
{
    QWidget* Widget = qobject_cast<QWidget*>(sender());
    if(!Widget){return;}

    int Index = ui->twChats->indexOf(Widget);
    ui->twChats->setTabText(Index,Name);
}

void MainWindow::SetClientStatus(ChatProtocol::Status Status)
{
    QWidget* Widget = qobject_cast<QWidget*>(sender());
    if(!Widget){return;}

    int Index = ui->twChats->indexOf(Widget);

    QString IconName = ":/Icons/";

    switch (Status) {
    case ChatProtocol::Available:
        IconName.append("available.png");
        break;
    case ChatProtocol::Away:
        IconName.append("away.png");
        break;
    case ChatProtocol::Busy:
        IconName.append("busy.png");
        break;
    default:
        IconName = "";
        break;
    }
    auto Icon = QIcon(IconName);
    ui->twChats->setTabIcon(Index,Icon);
}

void MainWindow::ClientDisconnected(QTcpSocket *ClientSocket)
{
    if(!ClientSocket){return;}

    QString ClientName = ClientSocket->property("ClientName").toString();
    ui->listClients->addItem(QString("Client " + ClientName + " disconnected!"));
}



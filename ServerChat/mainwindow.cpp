#include "clientchatwidget.h"
#include "mainwindow.h"
#include "servermanager.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QTcpSocket>
#include <QTimer>

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
    connect(ChatWidget, &ClientChatWidget::ClientChangedStatus,this,&MainWindow::SetClientStatus);
    connect(ChatWidget,&ClientChatWidget::ClientTyping,[this](QString Name){
        statusBar()->showMessage(Name,750);
    });

    connect(ChatWidget,&ClientChatWidget::TextForOtherClients,ServerManag.data(),&ServerManager::OnTextForOtherClients);
}

void MainWindow::SetClientName(const QString& OldName, const QString &Name)
{
    QWidget* Widget = qobject_cast<QWidget*>(sender());
    if(!Widget){return;}

    int Index = ui->twChats->indexOf(Widget);
    ui->twChats->setTabText(Index,Name);

    ServerManag->NotifyOtherClients(OldName,Name);
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

void MainWindow::on_btnDisconnectAll_clicked()
{
    if(ServerManag){
        ServerManag->DisconnectAllClients();
    }
}

void MainWindow::on_twChats_tabCloseRequested(int index)
{
    if(ClientChatWidget* ChatWidget = qobject_cast<ClientChatWidget*>(ui->twChats->widget(index))){
        ChatWidget->Disconnect();
        ui->twChats->removeTab(index);
    }
}

void MainWindow::DisplayImage(const QString& UserId, const QPixmap &pixmap)
{
    QLabel *Label;
    QTimer *Timer;

    if (UserLabels.contains(UserId)) {
        Label = UserLabels[UserId];
        Timer = UserTimers[UserId];
    } else {
        Label = new QLabel(this);
        Label->setObjectName(UserId);
        UserLabels[UserId] = Label;
        ui->vlVideos->addWidget(Label);

        Timer = new QTimer(this);
        Timer->setInterval(ClearLabelDelay);
        Timer->setSingleShot(true);
        connect(Timer, &QTimer::timeout, [=]() { RemoveUserLabel(UserId); });

        UserTimers[UserId] = Timer;
    }

    Label->setPixmap(pixmap.scaled(Label->size(), Qt::KeepAspectRatio));
    Timer->start();
}

void MainWindow::RemoveUserLabel(const QString &UserId)
{
    if (UserLabels.contains(UserId)) {
        QLabel *CurLabel = UserLabels.take(UserId);
        delete CurLabel;

        QTimer *CurTimer = UserTimers.take(UserId);
        delete CurTimer;
    }
}

#include "clientchatwidget.h"
#include "ui_clientchatwidget.h"
#include "clientmanager.h"
#include <QTcpSocket>


ClientChatWidget::ClientChatWidget(QTcpSocket *ClientSocket, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientChatWidget)
{
    ui->setupUi(this);
    Client.reset(new ClientManager(ClientSocket));
    ui->btnSend->setEnabled(false);

    connect(Client.data(),&ClientManager::Disconnected,this,&ClientChatWidget::ClientDisconnected);
    connect(Client.data(),&ClientManager::NameChanged,this,&ClientChatWidget::OnClientNameChanged);
    connect(Client.data(),&ClientManager::TextMessageReceived,this,&ClientChatWidget::TextMessageReceived);

}

ClientChatWidget::~ClientChatWidget()
{
    delete ui;
}

void ClientChatWidget::ClientDisconnected()
{
    ui->wdgSendTool->setEnabled(false);
}

void ClientChatWidget::OnClientNameChanged(const QString &OldName, const QString &NewName)
{
    emit ClientNameChanged(OldName, NewName);
}

void ClientChatWidget::TextMessageReceived(const QString &Message,const QString &Sender, const QString& Receiver)
{
    auto NewMessage = "<"+ Sender+"> " + Message;

    if(Receiver == "Server" || Receiver == "All"){
        ui->listChat->addItem(NewMessage);
    }
    if(Receiver != "Server"){
        emit TextForOtherClients(NewMessage, Client->GetName(),Receiver);
    }
}

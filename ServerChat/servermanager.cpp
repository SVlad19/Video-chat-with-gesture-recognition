#include "servermanager.h"
#include <QTcpServer>
#include <QTcpSocket>

ServerManager::ServerManager(QObject *parent)
    : QObject{parent}
{
    SetupServer();
}

void ServerManager::SetupServer()
{
    Server.reset(new QTcpServer(this));
    connect(Server.data(),&QTcpServer::newConnection,this,&ServerManager::NewClientConnected);

    Server->listen(QHostAddress::Any, 47000);
}

void ServerManager::NewClientConnected()
{
    QTcpSocket* ClientSocket = Server->nextPendingConnection();
    if(!ClientSocket){return;}

    int Id = ClientSockets.size() + 1;
    QString ClientName = QString("Client (%1)").arg(Id);

    ClientSocket->setProperty("Id",Id);
    ClientSocket->setProperty("ClientName",ClientName);

    connect(ClientSocket,&QTcpSocket::disconnected,this,&ServerManager::OnClientDisconnected);
    emit NewClient(ClientSocket);

    ClientSockets.push_back(ClientSocket);
}

void ServerManager::OnTextForOtherClients(const QString &Message, const QString &Sender, const QString &Receiver)
{
    QByteArray CurMessage = Protocol.SetTextMessage(Message,Receiver,"");

    if(Receiver == "All"){
        foreach (auto Client, ClientSockets) {
            QString ClientName = Client->property("ClientName").toString();
            if(ClientName != Sender){
                Client->write(CurMessage);
            }
        }
    }else{
        foreach (auto Client, ClientSockets) {
            auto ClientName = Client->property("ClientName").toString();
            if(ClientName == Receiver){
                Client->write(CurMessage);
                return;
            }
        }
    }
}

void ServerManager::OnClientDisconnected()
{
    QTcpSocket* ClientSocket = qobject_cast<QTcpSocket*>(sender());
    if(!ClientSocket) {return;}

    ClientSockets.removeOne(ClientSocket);
    emit ClientDisconnected(ClientSocket);
}

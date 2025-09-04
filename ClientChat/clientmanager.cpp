#include "clientmanager.h"
#include <QTcpSocket>

ClientManager::ClientManager(QCameraViewfinder *CameraViewfinder, QWidget *parent) {

    ServerSocket.reset(new QTcpSocket());
    connect(ServerSocket.data(),&QTcpSocket::connected,this,&ClientManager::Connected);
    connect(ServerSocket.data(),&QTcpSocket::disconnected,this,&ClientManager::Disconnected);
    connect(ServerSocket.data(),&QTcpSocket::readyRead,this,&ClientManager::ReadyRead);
}

ClientManager::~ClientManager() {
}

void ClientManager::ConnectToServer(const QString &IP)
{
    if(ServerSocket && ServerSocket->state() != QTcpSocket::ConnectedState){
        ServerSocket->connectToHost(IP,47000);
    }
}

void ClientManager::DisconnectFromServer()
{
    if(ServerSocket && ServerSocket->state() == QTcpSocket::ConnectedState){
        ServerSocket->disconnectFromHost();
    }
}

void ClientManager::ReadyRead()
{
    auto Data = ServerSocket->readAll();
    Protocol.LoadData(Data);

    switch (Protocol.GetDataType()) {
    case ChatProtocol::TextMessage:
        emit TextMessageReceived(Protocol.GetMessage());
        break;
    case ChatProtocol::ConnectionACK:
        emit ConncetionACK(Protocol.GetClientName(),Protocol.GetClientsName());
        break;
    case ChatProtocol::NewClientConnected:
        emit NewClientConnectedToServer(Protocol.GetClientName());
        break;
    case ChatProtocol::ClientChangedName:
        emit ClientChangedName(Protocol.GetOldClientName(),Protocol.GetClientName());
        break;
    default:
        break;
    }
}

void ClientManager::SendNewName(const QString &NewName)
{
    if(ServerSocket && ServerSocket->state() == QTcpSocket::ConnectedState){
        ServerSocket->write(Protocol.SetNameMessage(NewName));
    }
}

bool ClientManager::IsConnected() const
{
    if(!ServerSocket){ return false; }
    return ServerSocket->state() == QTcpSocket::ConnectedState;
}

void ClientManager::SendMessage(const QString &Message, const QString& Sender, const QString &Receiver)
{
    ServerSocket->write(Protocol.SetTextMessage(Message,Sender,Receiver));
}

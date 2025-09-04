#include "clientmanager.h"
#include <QTcpSocket>

ClientManager::ClientManager(QTcpSocket* Socket) {

    ClientSocket.reset(Socket);

    connect(ClientSocket.data(),&QTcpSocket::connected,this,&ClientManager::Connected);
    connect(ClientSocket.data(),&QTcpSocket::disconnected,this,&ClientManager::Disconnected);
    connect(ClientSocket.data(),&QTcpSocket::readyRead,this,&ClientManager::ReadyRead);
}

void ClientManager::ReadyRead()
{
    QByteArray Data = ClientSocket->readAll();
    Protocol.LoadData(Data);

    qDebug() << "Type = "<< Protocol.GetDataType();
    switch (Protocol.GetDataType()) {
    case ChatProtocol::ChangingName:
    {
        QString PrevName = ClientSocket->property("ClientName").toString();
        ClientSocket->setProperty("ClientName",GetName());
        emit NameChanged(PrevName,GetName());
        break;
    }
    case ChatProtocol::TextMessage:
    {
        emit TextMessageReceived(Protocol.GetMessage(),Protocol.GetClientName(),Protocol.GetReceiver());
        break;
    }
    default:
        break;
    }

}

QString ClientManager::GetName() const
{
    auto Id = ClientSocket->property("Id").toInt();
    auto Name = Protocol.GetClientName().length() > 0 ? Protocol.GetClientName() : QString("Client (%1)").arg(Id);

    return Name;
}

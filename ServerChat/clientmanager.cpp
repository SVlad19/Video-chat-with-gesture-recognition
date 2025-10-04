#include "clientmanager.h"
#include <QDataStream>
#include <QDateTime>
#include <QDir>
#include <QPixmap>
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
    Protocol.AppendData(Data);

    while (Protocol.HasFullPacket()) {
        Protocol.ParseNextPacket();

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
            emit TextMessageReceived(Protocol.GetClientMessage(),Protocol.GetClientName(),Protocol.GetReceiver());
            break;
        }
        case ChatProtocol::ClientChangeStatus:
            emit ClientChangedStatus(Protocol.GetClientStatus());
            break;
        case ChatProtocol::ClientTyping:
            emit ClientTyping();
            break;
        case ChatProtocol::InitSendingFile:
            emit InitReceivingFile(Protocol.GetClientName(), Protocol.GetFileName(), Protocol.GetFileSize());
            break;
        case ChatProtocol::FileChunk:
            SaveFileChunk();
            break;
        case ChatProtocol::VideoFrame:
            SaveVideoFrame();
            break;
        default:
            break;
        }
    }
}

QString ClientManager::GetName() const
{
    if(!ClientSocket) {return QString();}

    auto Id = ClientSocket->property("Id").toInt();
    auto Name = Protocol.GetClientName().length() > 0 ? Protocol.GetClientName() : QString("Client (%1)").arg(Id);

    return Name;
}

void ClientManager::SendClientTyping()
{
    if(ClientSocket){
         ClientSocket->write(Protocol.CreatePacket(ChatProtocol::ClientTyping));
    }
}

void ClientManager::DisconnectFromHost()
{
    if(ClientSocket){
        ClientSocket->disconnectFromHost();
    }
}

void ClientManager::SendResponseToReceiveFile(bool bAgreeToReceive)
{
    if(ClientSocket){
        ClientSocket->write(Protocol.CreatePacket(bAgreeToReceive ? ChatProtocol::AcceptSendingFile : ChatProtocol::RejecteSendingFile));
    }
}

void ClientManager::SaveFileChunk()
{
    if(!ClientSocket){return;}

    if (!bDirectoryCreated) {
        QDir Directory;
        Directory.mkdir(GetName());

        FilePath = QDir(GetName()).filePath(Protocol.GetFileName());
        File.reset(new QFile(FilePath));

        if (!File->open(QIODevice::WriteOnly)) {
            qDebug() << "Could not open file for writing!";
            return;
        }

        bDirectoryCreated = true;
    }

    File->write(Protocol.GetFileData());

    qint64 LastChunkSize = Protocol.GetFileData().size();
    ReceivedBytes += LastChunkSize;

    ClientSocket->write(Protocol.CreatePacket(ChatProtocol::ReadyForNextFileChunk, LastChunkSize));

    if (ReceivedBytes >= Protocol.GetFileSize()) {
        emit FileSavingFinished(FilePath);

        File->close();
        File.reset();
        ReceivedBytes = 0;
        bDirectoryCreated = false;
    }
}

void ClientManager::SaveVideoFrame()
{
    QByteArray Frame = Protocol.GetFrameData();
    QPixmap Pixmap;

    if (Pixmap.loadFromData(Frame)) {
        emit ImageReady(GetName(), Pixmap);
    } else {
        qDebug() << "Error with image";
        qDebug() << "Pixamp.loadFromData error";
    }
}

void ClientManager::SendMessage(const QString &Message)
{
    if(!ClientSocket){
        return;
    }
    ClientSocket->write(Protocol.CreatePacket(ChatProtocol::TextMessage,QString("Server"),GetName(), Message));
}

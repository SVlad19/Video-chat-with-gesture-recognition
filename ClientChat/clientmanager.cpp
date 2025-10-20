#include "cameramanager.h"
#include "clientmanager.h"
#include "filemanager.h"
#include "gesturerecognizer.h"

#include <QFileInfo>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

ClientManager::ClientManager(QWidget *parent) {

    ServerSocket.reset(new QTcpSocket());
    FileManag.reset(new FileManager(ServerSocket.data(),parent));

    connect(ServerSocket.data(),&QTcpSocket::connected,this,&ClientManager::Connected);
    connect(ServerSocket.data(),&QTcpSocket::disconnected,this,&ClientManager::Disconnected);
    connect(ServerSocket.data(),&QTcpSocket::readyRead,this,&ClientManager::ReadyRead);

    PythonThread = new QThread(this);
    Recognizer.reset(new GestureRecognizer());
    Recognizer->moveToThread(PythonThread);

    connect(PythonThread, &QThread::started, Recognizer.get(), &GestureRecognizer::StartProcess);
    connect(PythonThread, &QThread::finished, Recognizer.get(), &QObject::deleteLater);
    connect(this, &ClientManager::SendFrameToPythonWorker, Recognizer.get(), &GestureRecognizer::ProcessFrame);
    connect(Recognizer.get(), &GestureRecognizer::GestureRecognized, this, &ClientManager::GestureRecognized);

    PythonThread->start();
}

ClientManager::~ClientManager() {
    PythonThread->quit();
    PythonThread->wait();
    delete PythonThread;
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
    QByteArray Data = ServerSocket->readAll();
    Protocol.AppendData(Data);

    while (Protocol.HasFullPacket()) {
        Protocol.ParseNextPacket();

        switch (Protocol.GetDataType()) {
        case ChatProtocol::TextMessage:
            emit TextMessageReceived(Protocol.GetClientMessage());
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
        case ChatProtocol::ClientTyping:
            emit ClientTyping();
            break;
        case ChatProtocol::InitSendingFile:
            emit InitReceivingFile(Protocol.GetClientName(), Protocol.GetFileName(), Protocol.GetFileSize());
            break;
        case ChatProtocol::AcceptSendingFile:
            FileManag->ShowProgressBar();
            SendFile(0);
            break;
        case ChatProtocol::RejecteSendingFile:
            emit RejectReceivingFile();
            break;
        case ChatProtocol::ReadyForNextFileChunk:
            SendFile(Protocol.GetSentPackageSize());
            break;
        default:
            break;
        }
    }
}

void ClientManager::OnFrameReady(const QImage & Image, const QByteArray &Frame)
{
    if(ServerSocket){

        if (bRecognition) {
            PendingFrame = Image;
        }

        ServerSocket->write(Protocol.CreatePacket(ChatProtocol::VideoFrame,Frame));
    }
}

void ClientManager::SendNewName(const QString &NewName)
{
    if(ServerSocket && ServerSocket->state() == QTcpSocket::ConnectedState){
        ServerSocket->write(Protocol.CreatePacket(ChatProtocol::ChangingName,NewName));
    }
}

bool ClientManager::IsConnected() const
{
    if(!ServerSocket){ return false; }
    return ServerSocket->state() == QTcpSocket::ConnectedState;
}

void ClientManager::SendMessageToClient(const QString &Message, const QString& Sender, const QString &Receiver)
{
    if(ServerSocket && ServerSocket->state() == QTcpSocket::ConnectedState){
        ServerSocket->write(Protocol.CreatePacket(ChatProtocol::TextMessage, Sender, Receiver, Message));
    }
}

void ClientManager::SendStatus(ChatProtocol::Status Status)
{
    if(ServerSocket && ServerSocket->state() == QTcpSocket::ConnectedState){
        ServerSocket->write(Protocol.CreatePacket(ChatProtocol::ClientChangeStatus,Status));
    }
}

void ClientManager::SendClientTyping()
{
    if(ServerSocket && ServerSocket->state() == QTcpSocket::ConnectedState){
        ServerSocket->write(Protocol.CreatePacket(ChatProtocol::ClientTyping));
    }
}

void ClientManager::SendInitSendingFile(const QString &FileName)
{
    if( ServerSocket && FileManag && FileManag->FileSelected(FileName)){
        QFileInfo Info(FileName);
        ServerSocket->write(Protocol.CreatePacket(ChatProtocol::InitSendingFile,Info.fileName(), Info.size()));
    }
}

void ClientManager::SendResponseToReceiveFile(bool bAgreeToReceive)
{
    if(ServerSocket){
        ServerSocket->write(Protocol.CreatePacket(bAgreeToReceive ? ChatProtocol::MessageType::AcceptSendingFile : ChatProtocol::MessageType::RejecteSendingFile));
    }
}

void ClientManager::SendFile(qint64 Bytes)
{
    if(ServerSocket && FileManag){
        QByteArray FileChunk = FileManag->GetFileChunk(Bytes);
        if(!FileChunk.isEmpty()){
            ServerSocket->write(Protocol.CreatePacket(ChatProtocol::FileChunk,FileChunk));
        }
    }
}

void ClientManager::SetClientCamera(QCamera *ClientCamera)
{
    if(!CameraManag){
        CameraManag.reset(new CameraManager(ClientCamera));
        connect(CameraManag.get(), &CameraManager::FrameReady, this, &ClientManager::OnFrameReady);
    }else{
        CameraManag->SetClientCamera(ClientCamera);
    }
}

bool ClientManager::StartVideo()
{
    if(CameraManag){
        return CameraManag->StartVideo();
    }
    return false;
}

bool ClientManager::StopVideo()
{
    if(CameraManag){
        return CameraManag->StopVideo();
    }
    return false;
}

void ClientManager::ProcessPendingGestureFrame()
{
    if (!bRecognition || PendingFrame.isNull()) return;
    emit SendFrameToPythonWorker(PendingFrame);
}

/*
 * Gesture Recognizer
 */

void ClientManager::StartGestureRecognizer()
{
    if (!Recognizer || !PythonThread) return;

    bRecognition = true;

    if (!GestureTimer) {
        GestureTimer = new QTimer(this);
        connect(GestureTimer, &QTimer::timeout, this, &ClientManager::ProcessPendingGestureFrame);
    }

    GestureTimer->start(1000);
}

void ClientManager::StopGestureRecognizer()
{
    if (!Recognizer || !PythonThread) return;

    bRecognition = false;

    if (GestureTimer) {
        GestureTimer->stop();
        GestureTimer->deleteLater();
        GestureTimer = nullptr;
    }

}

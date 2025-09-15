#include "clientmanager.h"
#include "filemanager.h"

#include <QTcpSocket>
#include <QtMultimedia>
#include <QtMultimediaWidgets>

ClientManager::ClientManager(QCameraViewfinder *CameraViewfinder, QWidget *parent) {

    ServerSocket.reset(new QTcpSocket());
    FileManag.reset(new FileManager(ServerSocket.data(),parent));
    this->CameraViewfinder = CameraViewfinder;

    connect(ServerSocket.data(),&QTcpSocket::connected,this,&ClientManager::Connected);
    connect(ServerSocket.data(),&QTcpSocket::disconnected,this,&ClientManager::Disconnected);
    connect(ServerSocket.data(),&QTcpSocket::readyRead,this,&ClientManager::ReadyRead);

    SetupCamera();
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
    QByteArray Data = ServerSocket->readAll();
    Protocol.LoadData(Data);

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

void ClientManager::SendMessageToClient(const QString &Message, const QString& Sender, const QString &Receiver)
{
    if(ServerSocket && ServerSocket->state() == QTcpSocket::ConnectedState){
        ServerSocket->write(Protocol.SetTextMessage(Message,Sender,Receiver));
    }
}

void ClientManager::SendStatus(ChatProtocol::Status Status)
{
    if(ServerSocket && ServerSocket->state() == QTcpSocket::ConnectedState){
        ServerSocket->write(Protocol.SetStatusMessage(Status));
    }
}

void ClientManager::SendClientTyping()
{
    if(ServerSocket && ServerSocket->state() == QTcpSocket::ConnectedState){
        ServerSocket->write(Protocol.ClientTypingMessage());
    }
}

void ClientManager::SendInitSendingFile(const QString &FileName)
{
    if( ServerSocket && FileManag && FileManag->FileSelected(FileName)){
        ServerSocket->write(Protocol.SetInitSendingFileMessage(FileName));
    }
}

void ClientManager::SendResponseToReceiveFile(bool bAgreeToReceive)
{
    if(ServerSocket){
        ServerSocket->write(Protocol.SetResponseToReceiveFileMessage(bAgreeToReceive));
    }
}

void ClientManager::SendFile(qint64 Bytes)
{
    if(ServerSocket && FileManag){
        QByteArray FileChunk = FileManag->GetFileChunk(Bytes);
        if(!FileChunk.isEmpty()){
            ServerSocket->write(Protocol.SetFileChunkMessage(FileChunk));
        }
    }
}

void ClientManager::SetupCamera()
{
    if(!QCameraInfo::availableCameras().isEmpty()){
        Cameras = QCameraInfo::availableCameras();

        UserCamera.reset(new QCamera(QCameraInfo::defaultCamera()));
        UserCamera->setViewfinder(CameraViewfinder);

        ImageCapture.reset(new QCameraImageCapture(UserCamera.data()));
        connect(ImageCapture.get(), &QCameraImageCapture::imageCaptured, this, &ClientManager::OnImageCaptured);

        ImageCaptureTimer.reset(new QTimer());
        connect(ImageCaptureTimer.get(), &QTimer::timeout, this, &ClientManager::CaptureImage);
    }
}

void ClientManager::OnImageCaptured(int Id, const QImage &Image)
{
}

void ClientManager::CaptureImage()
{
}

QList<QCameraInfo>& ClientManager::GetCameras()
{
    return Cameras;
}

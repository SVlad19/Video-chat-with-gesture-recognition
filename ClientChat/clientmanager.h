#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include "../Utility/protocol.h"

class ClientManager : public QObject
{
    Q_OBJECT
public:
    ClientManager(QWidget *parent = nullptr);
    ~ClientManager();

    void ConnectToServer(const QString& IP);
    void DisconnectFromServer();
    void SendNewName(const QString& NewName);
    bool IsConnected() const;
    void SendMessageToClient(const QString& Message, const QString &Sender, const QString& Receiver);
    void SendStatus(ChatProtocol::Status Status);
    void SendClientTyping();
    void SendInitSendingFile(const QString& FileName);
    void SendResponseToReceiveFile(bool bAgreeToReceive);
    void SetClientCamera(class QCamera* ClientCamera);
    bool StartVideo();
    bool StopVideo();

signals:
    void Connected();
    void Disconnected();
    void TextMessageReceived(const QString& Message);
    void ConncetionACK(const QString& ClientName, QList<QString> ClientsName);
    void NewClientConnectedToServer(const QString& ClientName);
    void ClientChangedName(const QString& OldName, const QString& NewClientName);
    void ClientTyping();
    void InitReceivingFile(const QString& ClientName, const QString& FileName, qint64 FileSize);
    void RejectReceivingFile();

private slots:
    void ReadyRead();

private:
    void SendFile(qint64 Bytes);

    QScopedPointer<class QTcpSocket> ServerSocket;
    QScopedPointer<class FileManager> FileManag;
    QScopedPointer<class CameraManager> CameraManag;
    ChatProtocol Protocol;

};

#endif // CLIENTMANAGER_H

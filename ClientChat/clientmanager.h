#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include "../Utility/protocol.h"

class ClientManager : public QObject
{
    Q_OBJECT
public:
    ClientManager(class QCameraViewfinder *CameraViewfinder = nullptr, QWidget *parent = nullptr);
    ~ClientManager();

    void ConnectToServer(const QString& IP);
    void DisconnectFromServer();
    void SendNewName(const QString& NewName);
    bool IsConnected() const;
    void SendMessage(const QString& Message, const QString &Sender, const QString& Receiver);

signals:
    void Connected();
    void Disconnected();
    void TextMessageReceived(const QString& Message);

private slots:
    void ReadyRead();

private:
    QScopedPointer<class QTcpSocket> ServerSocket;
    ChatProtocol Protocol;
};

#endif // CLIENTMANAGER_H

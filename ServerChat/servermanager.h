#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include "../Utility/protocol.h"
#include <QMap>

class ServerManager : public QObject
{
    Q_OBJECT
public:
    explicit ServerManager(QObject *parent = nullptr);
    void NotifyOtherClients(const QString& OldName, const QString& Name);
    void DisconnectAllClients();

public slots:
    void OnTextForOtherClients(const QString& Message,const QString& Sender, const QString& Receiver);

signals:
    void NewClient(class QTcpSocket* ClientSocket);
    void ClientDisconnected(QTcpSocket* ClientSocket);

private slots:
    void NewClientConnected();
    void OnClientDisconnected();

private:
    void SetupServer();

    QScopedPointer<class QTcpServer> Server;
    QMap<QString, QTcpSocket*> ClientSockets;
    ChatProtocol Protocol;
};

#endif // SERVERMANAGER_H

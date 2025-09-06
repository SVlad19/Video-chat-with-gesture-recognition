#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include <QObject>
#include "../Utility/protocol.h"

class ClientManager : public QObject
{
    Q_OBJECT
public:
    ClientManager(class QTcpSocket* Socket);

    QString GetName()const;
    void SendClientTyping();

signals:
    void Connected();
    void Disconnected();
    void NameChanged(const QString& OldName, const QString& NewName);
    void TextMessageReceived(const QString& Message,const QString& Sender, const QString& Receiver);
    void ClientChangedStatus(ChatProtocol::Status Status);
    void ClientTyping();

private slots:
    void ReadyRead();

private:
    QScopedPointer<QTcpSocket> ClientSocket;
    ChatProtocol Protocol;
};

#endif // CLIENTMANAGER_H

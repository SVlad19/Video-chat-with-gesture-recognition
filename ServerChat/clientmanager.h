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
    void DisconnectFromHost();
    void SendResponseToReceiveFile(bool bAgreeToReceive);

signals:
    void Connected();
    void Disconnected();
    void NameChanged(const QString& OldName, const QString& NewName);
    void TextMessageReceived(const QString& Message,const QString& Sender, const QString& Receiver);
    void ClientChangedStatus(ChatProtocol::Status Status);
    void ClientTyping();
    void InitReceivingFile(const QString& ClientName, const QString& FileName, qint64 FileSize);
    void RejectReceivingFile();
    void FileSavingFinished(const QString& Path);

protected:
    void SaveFileChunk();

private slots:
    void ReadyRead();

private:
    QScopedPointer<QTcpSocket> ClientSocket;
    ChatProtocol Protocol;

    // File

    bool bDirectoryCreated = false;
    qint64 ReceivedBytes = 0;
    QString FilePath;
    QScopedPointer<class QFile> File;
};

#endif // CLIENTMANAGER_H

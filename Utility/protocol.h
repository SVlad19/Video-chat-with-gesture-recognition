#pragma once
#include "QObject"

#include <QIODevice>
#include <QDataStream>

class ChatProtocol{
public:

    enum MessageType{
        ChangingName,
        TextMessage,
        ConnectionACK,
        NewClientConnected,
        ClientChangedName,
        ClientChangeStatus,
        ClientTyping,
        InitSendingFile,
        SendFile,
        AcceptSendingFile,
        RejecteSendingFile,
        FileChunk,
        ReadyForNextFileChunk
    };

    enum Status{
        None,
        Available,
        Away,
        Busy
    };

    void LoadData(QByteArray Data);

    QByteArray SetNameMessage(const QString& ClientName);
    QByteArray SetTextMessage(const QString &Message, const QString &Receiver, const QString &Sender);
    QByteArray SetConnectionACKMessage(const QString& ClientName, QList<QString> OtherClients);
    QByteArray SetNewClientConnectedMessage(const QString& ClientName);
    QByteArray SetClientChangedNameMessage(const QString& OldName, const QString& Name);
    QByteArray SetStatusMessage(ChatProtocol::Status Status);
    QByteArray ClientTypingMessage();
    QByteArray SetInitSendingFileMessage(const QString& FileName);
    QByteArray SetResponseToReceiveFileMessage(bool bAgreeToReceive);
    QByteArray SetFileChunkMessage(QByteArray& FileData);
    QByteArray SetReadyForNextFileChunkMessage(qint64 Bytes);

    inline MessageType GetDataType() const{return DataType;}
    inline QString GetClientName() const{return ClientName;}
    inline QString GetMessage() const {return Message;}
    inline QString GetReceiver() const{return Receiver;}
    inline QList<QString> GetClientsName()const {return ClientsName;}
    inline QString GetOldClientName() const {return OldClientName;};
    inline ChatProtocol::Status GetClientStatus() const {return ClientStatus;}
    inline QString GetFileName() const { return FileName; }
    inline qint64 GetFileSize() const {return FileSize; }
    inline qint64 GetSentPackageSize() const{ return SentPackageSize; }
    inline QByteArray GetFileData() const{ return FileData; }

private:
    template<typename T>
    QByteArray GetData(const MessageType Type, const T& Message);

    MessageType DataType;
    QString ClientName;
    QString OldClientName;
    QString Receiver;
    QString Message;
    QString FileName;
    qint64 FileSize;
    QByteArray FileData;
    qint64 SentPackageSize = 0;
    ChatProtocol::Status ClientStatus;
    QList<QString> ClientsName;

};

template<typename T>
QByteArray ChatProtocol::GetData(const MessageType Type, const T &Message)
{
    QByteArray CurMessage;
    QDataStream Out(&CurMessage, QIODevice::WriteOnly);
    Out.setVersion(QDataStream::Version::Qt_5_15);

    Out << Type << Message;

    return CurMessage;
}

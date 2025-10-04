#pragma once
#include "QObject"
#include "qdebug.h"

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
        ReadyForNextFileChunk,
        VideoFrame
    };

    enum Status{
        None,
        Available,
        Away,
        Busy
    };

    template<typename... Args>
    QByteArray CreatePacket(MessageType Type, const Args&... _Args)
    {
        QByteArray Block;
        QDataStream Out(&Block, QIODevice::WriteOnly);
        Out.setVersion(QDataStream::Qt_5_15);

        Out << quint32(0) << Type;
        (Out << ... << _Args);

        Out.device()->seek(0);
        Out << quint32(Block.size() - sizeof(quint32));

        return Block;
    }

    bool HasFullPacket() const;
    void ParseNextPacket();
    void AppendData(const QByteArray& data);

    inline MessageType GetDataType() const{return DataType;}
    inline QString GetClientName() const{return ClientName;}
    inline QString GetClientMessage() const {return Message;}
    inline QString GetReceiver() const{return Receiver;}
    inline QList<QString> GetClientsName()const {return ClientsName;}
    inline QString GetOldClientName() const {return OldClientName;};
    inline ChatProtocol::Status GetClientStatus() const {return ClientStatus;}
    inline QString GetFileName() const { return FileName; }
    inline qint64 GetFileSize() const {return FileSize; }
    inline qint64 GetSentPackageSize() const{ return SentPackageSize; }
    inline QByteArray GetFileData() const{ return FileData; }
    inline QByteArray GetFrameData() const {return FrameData; }

private:

    QByteArray Buffer;
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
    QByteArray FrameData;
    QList<QString> ClientsName;

};


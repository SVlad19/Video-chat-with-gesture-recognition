#include "protocol.h"

#include <QFileInfo>

void ChatProtocol::LoadData(QByteArray Data)
{
    QDataStream In(&Data, QIODevice::ReadOnly);
    In.setVersion(QDataStream::Version::Qt_5_15);
    In >> DataType;

    switch (DataType) {
    case ChangingName:
        In >> ClientName;
        break;
    case TextMessage:
        In >> ClientName >> Receiver >> Message;
        break;
    case ConnectionACK:
        In >> ClientName >> ClientsName;
        break;
    case NewClientConnected:
        In >> ClientName;
        break;
    case ClientChangedName:
        In >> OldClientName >> ClientName;
        break;
    case ClientChangeStatus:
        In >> ClientStatus;
        break;
    case InitSendingFile:
        In >> FileName >> FileSize;
        break;
    case ChatProtocol::ReadyForNextFileChunk:
        In >> SentPackageSize;
        break;
    case ChatProtocol::FileChunk:
        In >> FileData;
        break;
    default:
        break;
    }
}

QByteArray ChatProtocol::SetNameMessage(const QString &ClientName)
{
    return GetData(MessageType::ChangingName,ClientName);
}

QByteArray ChatProtocol::SetTextMessage(const QString &Message, const QString &Sender, const QString &Receiver)
{
    QByteArray CurMessage;
    QDataStream Out(&CurMessage, QIODevice::WriteOnly);
    Out.setVersion(QDataStream::Version::Qt_5_15);

    Out << TextMessage << Sender << Receiver << Message;

    return CurMessage;
}

QByteArray ChatProtocol::SetConnectionACKMessage(const QString &ClientName, QList<QString> OtherClients)
{
    QByteArray CurMessage;
    QDataStream Out(&CurMessage, QIODevice::WriteOnly);
    Out.setVersion(QDataStream::Version::Qt_5_15);

    Out << ConnectionACK << ClientName << OtherClients;

    return CurMessage;
}

QByteArray ChatProtocol::SetNewClientConnectedMessage(const QString &ClientName)
{
    return GetData(MessageType::NewClientConnected,ClientName);
}

QByteArray ChatProtocol::SetClientChangedNameMessage(const QString &OldName, const QString &Name)
{
    QByteArray CurMessage;
    QDataStream Out(&CurMessage, QIODevice::WriteOnly);
    Out.setVersion(QDataStream::Version::Qt_5_15);

    Out << ClientChangedName << OldName << Name;

    return CurMessage;
}

QByteArray ChatProtocol::SetStatusMessage(Status Status)
{
    return GetData(MessageType::ClientChangeStatus,Status);
}

QByteArray ChatProtocol::ClientTypingMessage()
{
    return GetData(MessageType::ClientTyping,"");
}

QByteArray ChatProtocol::SetInitSendingFileMessage(const QString &FileName)
{
    QByteArray CurMessage;
    QDataStream Out(&CurMessage, QIODevice::WriteOnly);
    Out.setVersion(QDataStream::Version::Qt_5_15);

    QFileInfo Info(FileName);
    Out << InitSendingFile << Info.fileName() << Info.size();

    return CurMessage;
}

QByteArray ChatProtocol::SetResponseToReceiveFileMessage(bool bAgreeToReceive)
{
    return bAgreeToReceive ? GetData(MessageType::AcceptSendingFile,"") : GetData(MessageType::RejecteSendingFile,"");
}

QByteArray ChatProtocol::SetFileChunkMessage(QByteArray& FileData)
{
    return GetData(MessageType::FileChunk, FileData);
}

QByteArray ChatProtocol::SetReadyForNextFileChunkMessage(qint64 Bytes)
{
    return GetData(MessageType::ReadyForNextFileChunk, Bytes);
}


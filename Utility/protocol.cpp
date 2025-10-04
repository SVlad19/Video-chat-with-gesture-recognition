#include "protocol.h"
#include "qdebug.h"

#include <QFileInfo>

bool ChatProtocol::HasFullPacket() const
{
    if (Buffer.size() < sizeof(quint32)){
        return false;
    }

    QDataStream In(Buffer);
    In.setVersion(QDataStream::Qt_5_15);
    quint32 Size = 0;
    In >> Size;

    return Buffer.size() >= Size + sizeof(quint32);
}

void ChatProtocol::AppendData(const QByteArray& data)
{
    Buffer.append(data);
}

void ChatProtocol::ParseNextPacket()
{
    if (!HasFullPacket()){
        return;
    }

    QDataStream In(&Buffer, QIODevice::ReadOnly);
    In.setVersion(QDataStream::Qt_5_15);

    quint32 Size = 0;
    In >> Size;

    QByteArray Block = Buffer.mid(sizeof(quint32), Size);
    Buffer.remove(0, sizeof(quint32) + Size);

    QDataStream PacketStream(&Block, QIODevice::ReadOnly);
    PacketStream.setVersion(QDataStream::Qt_5_15);
    PacketStream >> DataType;

    switch (DataType) {
    case ChangingName:
        PacketStream >> ClientName;
        break;
    case TextMessage:
        PacketStream >> ClientName >> Receiver >> Message;
        break;
    case ConnectionACK:
        PacketStream >> ClientName >> ClientsName;
        break;
    case NewClientConnected:
        PacketStream >> ClientName;
        break;
    case ClientChangedName:
        PacketStream >> OldClientName >> ClientName;
        break;
    case ClientChangeStatus:
        PacketStream >> ClientStatus;
        break;
    case InitSendingFile:
        PacketStream >> FileName >> FileSize;
        break;
    case ChatProtocol::ReadyForNextFileChunk:
        PacketStream >> SentPackageSize;
        break;
    case ChatProtocol::FileChunk:
        PacketStream >> FileData;
        break;
    case ChatProtocol::VideoFrame:
        PacketStream >> FrameData;
        break;
    default:
        break;
    }
}



#include "protocol.h"

#include <QDataStream>
#include <QIODevice>

void ChatProtocol::LoadData(QByteArray Data)
{
    QDataStream in(&Data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Version::Qt_5_15);
    in >> DataType;

    switch (DataType) {
    case ChangingName:
        in >> ClientName;
        break;
    default:
        break;
    }
}

QByteArray ChatProtocol::GetData(const MessageType Type, const QString &Message)
{
    QByteArray CurMessage;
    QDataStream out(&CurMessage, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version::Qt_5_15);

    out << Type << Message;

    return CurMessage;
}

QByteArray ChatProtocol::SetNameMessage(const QString &ClientName)
{
    return GetData(MessageType::ChangingName,ClientName);
}

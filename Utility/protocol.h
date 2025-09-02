#pragma once
#include "QObject"

class ChatProtocol{
public:

    enum MessageType{
        ChangingName,
        TextMessage
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

    inline MessageType GetDataType() const{return DataType;}
    inline QString GetName() const{return ClientName;}
    inline QString GetMessage() const {return Message;}
    inline QString GetReceiver() const{return Receiver;}

private:
    QByteArray GetData(const MessageType Type, const QString& Message);

    MessageType DataType;
    QString ClientName;
    QString Receiver;
    QString Message;

};


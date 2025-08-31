#pragma once
#include "QObject"

class ChatProtocol{
public:

    enum MessageType{
        ChangingName
    };

    enum Status{
        None,
        Available,
        Away,
        Busy
    };

    void LoadData(QByteArray Data);
    QByteArray SetNameMessage(const QString& ClientName);

    inline MessageType GetDataType() const{return DataType;}
    inline QString GetName() const{return ClientName;}

private:
    QByteArray GetData(const MessageType Type, const QString& Message);

    MessageType DataType;
    QString ClientName;
};


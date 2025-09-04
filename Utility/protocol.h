#pragma once
#include "QObject"

class ChatProtocol{
public:

    enum MessageType{
        ChangingName,
        TextMessage,
        ConnectionACK,
        NewClientConnected,
        ClientChangedName
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

    inline MessageType GetDataType() const{return DataType;}
    inline QString GetClientName() const{return ClientName;}
    inline QString GetMessage() const {return Message;}
    inline QString GetReceiver() const{return Receiver;}
    inline QList<QString> GetClientsName()const {return ClientsName;}
    inline QString GetOldClientName() const {return OldClientName;};

private:
    QByteArray GetData(const MessageType Type, const QString& Message);

    MessageType DataType;
    QString ClientName;
    QString OldClientName;
    QString Receiver;
    QString Message;
    QList<QString> ClientsName;

};


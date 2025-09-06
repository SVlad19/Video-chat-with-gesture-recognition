#ifndef CLIENTCHATWIDGET_H
#define CLIENTCHATWIDGET_H

#include <QWidget>
#include "../Utility/protocol.h"

namespace Ui {
class ClientChatWidget;
}

class ClientChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ClientChatWidget(class QTcpSocket* ClientSocket, QWidget *parent = nullptr);
    ~ClientChatWidget();

signals:
    void ClientNameChanged(const QString& OldName, const QString& Name);
    void ClientTyping(const QString& Message);
    void ClientChangedStatus(ChatProtocol::Status Status);
    void TextForOtherClients(const QString& Message,const QString &Sender, const QString &Receiver);

private slots:
    void ClientDisconnected();
    void OnClientNameChanged(const QString& OldName, const QString& NewName);
    void TextMessageReceived(const QString& Message,const QString &Sender, const QString &Receiver);
    void OnClientTyping();

private:
    Ui::ClientChatWidget *ui;
    QScopedPointer<class ClientManager> Client;
};

#endif // CLIENTCHATWIDGET_H

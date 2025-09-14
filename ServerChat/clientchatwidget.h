#ifndef CLIENTCHATWIDGET_H
#define CLIENTCHATWIDGET_H

#include <QDir>
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

    void Disconnect();

signals:    
    void ClientNameChanged(const QString& OldName, const QString& Name);
    void ClientTyping(const QString& Message);
    void ClientChangedStatus(ChatProtocol::Status Status);
    void TextForOtherClients(const QString& Message,const QString &Sender, const QString &Receiver);

protected:
    void SendMessage();

private slots:
    void on_lOpenFolder_linkActivated(const QString &link);
    void on_leMessage_textChanged(const QString &arg1);
    void on_leMessage_returnPressed();
    void on_btnSend_clicked();

    void ClientDisconnected();
    void OnClientNameChanged(const QString& OldName, const QString& NewName);
    void TextMessageReceived(const QString& Message,const QString &Sender, const QString &Receiver);
    void OnClientTyping();
    void OnInitReceivingFile(const QString& ClientName, const QString& FileName, qint64 FileSize);
    void OnFileSavingFinished(const QString& FilePath);

private:
    Ui::ClientChatWidget *ui;
    QScopedPointer<class ClientManager> Client;
    QDir Directiory;
};

#endif // CLIENTCHATWIDGET_H

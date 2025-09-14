#include "clientchatwidget.h"
#include "ui_clientchatwidget.h"
#include "clientmanager.h"
#include <QMessageBox>
#include <QTcpSocket>


ClientChatWidget::ClientChatWidget(QTcpSocket *ClientSocket, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientChatWidget)
{
    ui->setupUi(this);
    Client.reset(new ClientManager(ClientSocket));
    ui->btnSend->setEnabled(false);

    connect(Client.data(),&ClientManager::Disconnected,this,&ClientChatWidget::ClientDisconnected);
    connect(Client.data(),&ClientManager::NameChanged,this,&ClientChatWidget::OnClientNameChanged);
    connect(Client.data(),&ClientManager::TextMessageReceived,this,&ClientChatWidget::TextMessageReceived);
    connect(Client.data(),&ClientManager::ClientChangedStatus,this,&ClientChatWidget::ClientChangedStatus);
    connect(Client.data(),&ClientManager::ClientTyping,this,&ClientChatWidget::OnClientTyping);
    connect(Client.data(),&ClientManager::InitReceivingFile,this,&ClientChatWidget::OnInitReceivingFile);
    connect(Client.data(),&ClientManager::FileSavingFinished,this,&ClientChatWidget::OnFileSavingFinished);

    connect(ui->leMessage,&QLineEdit::textChanged,Client.data(),&ClientManager::SendClientTyping);

}

ClientChatWidget::~ClientChatWidget()
{
    delete ui;
}

void ClientChatWidget::OnClientTyping()
{
    emit ClientTyping(QString("%1 is typing...").arg(Client->GetName()));
}

void ClientChatWidget::ClientDisconnected()
{
    ui->wdgSendTool->setEnabled(false);
}

void ClientChatWidget::OnClientNameChanged(const QString &OldName, const QString &NewName)
{
    emit ClientNameChanged(OldName, NewName);
}

void ClientChatWidget::TextMessageReceived(const QString &Message,const QString &Sender, const QString& Receiver)
{
    QString NewMessage = "<"+ Sender+"> " + Message;

    if(Receiver == "Server" || Receiver == "All"){
        ui->listChat->addItem(NewMessage);
    }
    if(Receiver != "Server"){
        emit TextForOtherClients(NewMessage, Client->GetName(),Receiver);
    }
}

void ClientChatWidget::Disconnect()
{
    Client->DisconnectFromHost();
}

void ClientChatWidget::OnInitReceivingFile(const QString &ClientName, const QString &FileName, qint64 FileSize)
{
    if(!Client){
        return;
    }

    QString Message = QString("Client (%1) wants to send a file. Do you want accept it?\nFile Name: %2\nFile Size: %3 bytes.").arg(ClientName,FileName).arg(FileSize);
    auto Result = QMessageBox::question(this,"Receiving File", Message);

    Client->SendResponseToReceiveFile(Result == QMessageBox::Yes ? true : false);
}

void ClientChatWidget::OnFileSavingFinished(const QString &FilePath)
{
    QString Message = QString("File saved here:\n%1").arg(FilePath);
    QMessageBox::information(this,"File saved",Message);
}

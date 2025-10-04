#include "clientchatwidget.h"
#include "ui_clientchatwidget.h"
#include "clientmanager.h"
#include <MainWindow.h>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTcpSocket>
#include <QUrl>


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
    connect(Client.data(),&ClientManager::ImageReady,dynamic_cast<MainWindow*>(parent),&MainWindow::DisplayImage);

    connect(ui->leMessage,&QLineEdit::textChanged,Client.data(),&ClientManager::SendClientTyping);


    Directiory.mkdir(Client->GetName());
    Directiory.setPath("./" + Client->GetName());
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
    QFile::rename(Directiory.canonicalPath(),NewName);
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
    if(Client){
        Client->DisconnectFromHost();
    }
}

void ClientChatWidget::OnInitReceivingFile(const QString &ClientName, const QString &FileName, qint64 FileSize)
{
    if(Client){
        QString Message = QString("Client (%1) wants to send a file. Do you want accept it?\nFile Name: %2\nFile Size: %3 bytes.").arg(ClientName,FileName).arg(FileSize);
        auto Result = QMessageBox::question(this,"Receiving File", Message);

        Client->SendResponseToReceiveFile(Result == QMessageBox::Yes ? true : false);
    }
}

void ClientChatWidget::OnFileSavingFinished(const QString &FilePath)
{
    QString Message = QString("File saved here:\n%1").arg(FilePath);
    QMessageBox::information(this,"File saved",Message);
}

void ClientChatWidget::SendMessage()
{
    if(ui->leMessage->text().isEmpty() || !Client){
        return;
    }
    QString Message = "<Server> " + ui->leMessage->text().trimmed().toUtf8();

    Client->SendMessage(Message);

    ui->leMessage->clear();
    ui->listChat->addItem(Message);
}

void ClientChatWidget::on_lOpenFolder_linkActivated(const QString &link)
{
    if(Client){
        QDesktopServices::openUrl(QUrl::fromLocalFile(Client->GetName()));
    }
}

void ClientChatWidget::on_leMessage_textChanged(const QString &arg1)
{
    ui->btnSend->setEnabled(arg1.trimmed().length() > 0);
}

void ClientChatWidget::on_leMessage_returnPressed()
{
    SendMessage();
}

void ClientChatWidget::on_btnSend_clicked()
{
    SendMessage();
}

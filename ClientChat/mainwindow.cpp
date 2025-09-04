#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectionwidget.h"
#include "clientmanager.h"
#include "chatitemwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionDisconnect->setEnabled(false);
    ui->lineClientName->setText("Client");
    ui->centralwidget->setEnabled(false);

    ConnectionWindowWidget.reset(new ConnectionWidget());
    connect(ConnectionWindowWidget.data(),&ConnectionWidget::ConnectionDataReady,this,&MainWindow::HandleConnectionData);

    SetupClient();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupClient()
{
    Client.reset(new ClientManager(nullptr,this));

    connect(Client.data(), &ClientManager::Connected, [this](){
        ui->centralwidget->setEnabled(true);
        ui->actionConnect->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    });

    connect(Client.data(), &ClientManager::Disconnected, [this](){
        ui->centralwidget->setEnabled(false);
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
    });
    connect(Client.data(), &ClientManager::TextMessageReceived, this, &MainWindow::OnTextMessageReceived);
    connect(Client.data(), &ClientManager::ConncetionACK, this, &MainWindow::OnConncetionACK);
    connect(Client.data(), &ClientManager::NewClientConnectedToServer, this, &MainWindow::OnNewClientConnectedToServer);
    connect(Client.data(), &ClientManager::ClientChangedName, this, &MainWindow::OnClientChangedName);
}

void MainWindow::OnClientChangedName(const QString &OldName, const QString &NewClientName)
{
    for(int i = 0; i < ui->cbDestination->count(); ++i){
        if(ui->cbDestination->itemText((i)) == OldName){
            ui->cbDestination->setItemText(i,NewClientName);
            return;
        }
    }
}

void MainWindow::OnConncetionACK(const QString &ClientName, QList<QString> ClientsName)
{
    ui->cbDestination->clear();
    ClientsName.prepend("All");
    ClientsName.prepend("Server");

    foreach(auto Client, ClientsName){
        ui->cbDestination->addItem(Client);
    }
}

void MainWindow::OnNewClientConnectedToServer(const QString &ClientName)
{
    ui->cbDestination->addItem(ClientName);
}

void MainWindow::OnTextMessageReceived(const QString& Message)
{
    ChatItemWidget* ChatWidget = new ChatItemWidget();
    ChatWidget->SetMessage(Message);

    QListWidgetItem* ListItem = new QListWidgetItem();
    ListItem->setSizeHint(QSize(0,65));
    ListItem->setBackground(QColor(167,255,237));

    ui->listChat->addItem(ListItem);
    ui->listChat->setItemWidget(ListItem,ChatWidget);
}

void MainWindow::HandleConnectionData(const QString &IP)
{
    if(Client){
        Client->ConnectToServer(IP);
    }
}

void MainWindow::SendMessage()
{
    QString Message = ui->leMessage->text().trimmed();
    if(Message.isEmpty()){
        return;
    }

    Client->SendMessage(Message,ui->lineClientName->text(),ui->cbDestination->currentText());

    ui->leMessage->clear();
    ui->leMessage->setFocus();

    ChatItemWidget* ChatWidget = new ChatItemWidget();
    ChatWidget->SetMessage(Message,true);

    QListWidgetItem* ListItem = new QListWidgetItem();
    ListItem->setSizeHint(QSize(0,65));

    ui->listChat->addItem(ListItem);
    ui->listChat->setItemWidget(ListItem,ChatWidget);
}

void MainWindow::on_actionConnect_triggered()
{
    if(ConnectionWindowWidget){
        ConnectionWindowWidget->show();
    }
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}


void MainWindow::on_actionDisconnect_triggered()
{
    if(Client){
        Client->DisconnectFromServer();
    }
}


void MainWindow::on_lineClientName_editingFinished()
{
    if(Client){
        QString Name = ui->lineClientName->text().trimmed();
        Client->SendNewName(Name);
    }
}


void MainWindow::on_btnSend_clicked()
{
    SendMessage();
}


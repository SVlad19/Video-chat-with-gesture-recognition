#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectionwidget.h"
#include "clientmanager.h"
#include "chatitemwidget.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCameraInfo>
#include <QtMultimedia>
#include <QtMultimediaWidgets>

Q_DECLARE_METATYPE(QCameraInfo)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->actionDisconnect->setEnabled(false);
    ui->lineClientName->setText("Client");
    ui->centralwidget->setEnabled(false);
    ui->wCameraViewfinder->setAutoFillBackground(true);

    ConnectionWindowWidget.reset(new ConnectionWidget());
    connect(ConnectionWindowWidget.data(),&ConnectionWidget::ConnectionDataReady,this,&MainWindow::HandleConnectionData);

    SetupClient();
    SetupCamera();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupClient()
{
    Client.reset(new ClientManager(this));

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
    connect(Client.data(), &ClientManager::ClientTyping, this, &MainWindow::OnClientTyping);
    connect(Client.data(), &ClientManager::InitReceivingFile, this, &MainWindow::OnInitReceivingFile);
    connect(Client.data(), &ClientManager::RejectReceivingFile, this, &MainWindow::OnRejectReceivingFile);
}

void MainWindow::SetupCamera()
{
    if(Client && !QCameraInfo::availableCameras().isEmpty()){
        QList<QCameraInfo> Cameras = QCameraInfo::availableCameras();

        foreach (auto& Camera, Cameras) {
            ui->cbCameras->addItem(Camera.description(),QVariant::fromValue(Camera));
        }

        QCamera* CurrentCamera = new QCamera(QCameraInfo::defaultCamera());
        CurrentCamera->setViewfinder(ui->wCameraViewfinder);

        Client->SetClientCamera(CurrentCamera);
    }
}

void MainWindow::StartVideo()
{
    if(Client && !Client->StartVideo()){
        QMessageBox::critical(this,"Warning","Camera not detected!");
    }else{
        ui->wCameraViewfinder->setStyleSheet("background-color: black; color: white;");
        ui->btnStartVideo->setEnabled(false);
        ui->btnStopVideo->setEnabled(true);
        ui->lineClientName->setEnabled(false);
        ui->HSliderFrameRate->setEnabled(false);
        ui->btnSendFile->setEnabled(false);
        ui->checkBox->setEnabled(true);
        ui->cbCameras->setEnabled(false);
    }
}

void MainWindow::StopVideo()
{
    if(Client && !Client->StopVideo()){
        QMessageBox::critical(this,"Warning","Camera not detected!");
    }else{
        ui->btnStartVideo->setEnabled(true);
        ui->btnStopVideo->setEnabled(false);
        ui->lineClientName->setEnabled(true);
        ui->HSliderFrameRate->setEnabled(true);
        ui->btnSendFile->setEnabled(true);
        ui->checkBox->setChecked(false);
        ui->checkBox->setEnabled(false);
        ui->cbCameras->setEnabled(true);
    }
}

void MainWindow::OnInitReceivingFile(const QString &ClientName, const QString &FileName, qint64 FileSize)
{
    QString Message = QString("Client (%1) wants to send a file. Do you want accept it?\nFile Name: %2\nFile Size: %3 bytes.").arg(ClientName,FileName).arg(FileSize);
    auto Result = QMessageBox::question(this,"Receiving File", Message);

    Client->SendResponseToReceiveFile(Result == QMessageBox::Yes ? true : false);
}

void MainWindow::OnClientTyping()
{
    statusBar()->showMessage("Server is typing...",750);
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

void MainWindow::SendMessageToClient()
{
    QString Message = ui->leMessage->text().trimmed();
    if(Message.isEmpty()){
        return;
    }

    Client->SendMessageToClient(Message,ui->lineClientName->text(),ui->cbDestination->currentText());

    ui->leMessage->clear();
    ui->leMessage->setFocus();

    ChatItemWidget* ChatWidget = new ChatItemWidget();
    ChatWidget->SetMessage(Message,true);

    QListWidgetItem* ListItem = new QListWidgetItem();
    ListItem->setSizeHint(QSize(0,65));

    ui->listChat->addItem(ListItem);
    ui->listChat->setItemWidget(ListItem,ChatWidget);
}

void MainWindow::OnRejectReceivingFile()
{
    QMessageBox::critical(this, "Sending file", "Operation rejected...");
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
    SendMessageToClient();
}

void MainWindow::on_cbStatus_currentIndexChanged(int index)
{
    if(Client){
        ChatProtocol::Status Status = static_cast<ChatProtocol::Status>(index);
        Client->SendStatus(Status);
    }
}

void MainWindow::on_leMessage_textChanged(const QString &arg1)
{
    if(Client){
        ui->btnSend->setEnabled(arg1.trimmed().length() > 0);
        Client->SendClientTyping();
    }
}

void MainWindow::on_btnSendFile_clicked()
{
    if(Client){
        QString FileName = QFileDialog::getOpenFileName(this,"Select a file", "D:/");
        Client->SendInitSendingFile(FileName);
    }
}

void MainWindow::on_btnStartVideo_clicked()
{
    StartVideo();
}

void MainWindow::on_btnStopVideo_clicked()
{
    StopVideo();
}

void MainWindow::on_cbCameras_currentIndexChanged(int index)
{
    if(Client){
        QCamera* NewCamera = new QCamera(ui->cbCameras->itemData(index).value<QCameraInfo>());
        if(NewCamera){
            NewCamera->setViewfinder(ui->wCameraViewfinder);


            Client->SetClientCamera(NewCamera);
        }
    }
}

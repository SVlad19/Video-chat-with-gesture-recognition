#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionConnect_triggered();
    void on_actionExit_triggered();
    void on_actionDisconnect_triggered();
    void on_lineClientName_editingFinished();
    void on_btnSend_clicked();
    void on_cbStatus_currentIndexChanged(int index);
    void on_leMessage_textChanged(const QString &arg1);
    void on_btnSendFile_clicked();
    void on_btnStartVideo_clicked();
    void on_btnStopVideo_clicked();

    void HandleConnectionData(const QString& IP);
    void OnTextMessageReceived(const QString& Message);
    void OnConncetionACK(const QString& ClientName, QList<QString> ClientsName);
    void OnNewClientConnectedToServer(const QString& ClientName);
    void OnClientChangedName(const QString& OldName, const QString& NewClientName);
    void OnClientTyping();
    void OnInitReceivingFile(const QString& ClientName, const QString& FileName, qint64 FileSize);
    void OnRejectReceivingFile();

private:
    void SetupClient();
    void SetupCamera();
    void SendMessageToClient();

    Ui::MainWindow *ui;
    QScopedPointer<class ConnectionWidget> ConnectionWindowWidget;
    QScopedPointer<class ClientManager> Client;
};
#endif // MAINWINDOW_H

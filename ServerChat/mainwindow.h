#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Utility/protocol.h"

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

    void on_twChats_tabCloseRequested(int index);
    void on_btnDisconnectAll_clicked();

    void NewClient(class QTcpSocket* ClientSocket);
    void ClientDisconnected(class QTcpSocket* ClientSocket);
    void SetClientName(const QString &OldName, const QString& NewName);
    void SetClientStatus(const ChatProtocol::Status Status);

private:
    Ui::MainWindow *ui;
    QScopedPointer<class ServerManager> ServerManag;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../Utility/protocol.h"
#include <QLabel>
#include <QMap>

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

public slots:
    void DisplayImage(const QString& UserID, const QPixmap &pixmap);

protected:
    void RemoveUserLabel(const QString &UserId);

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
    QMap<QString, QLabel*> UserLabels;
    QMap<QString, QTimer*> UserTimers;
    int ClearLabelDelay = 1100;
};
#endif // MAINWINDOW_H


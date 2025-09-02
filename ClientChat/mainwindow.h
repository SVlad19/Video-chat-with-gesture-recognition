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

    void HandleConnectionData(const QString& IP);
    void OnTextMessageReceived(const QString& Message);

private:
    void SetupClient();
    void SendMessage();

    Ui::MainWindow *ui;
    QScopedPointer<class ConnectionWidget> ConnectionWindowWidget;
    QScopedPointer<class ClientManager> Client;
};
#endif // MAINWINDOW_H

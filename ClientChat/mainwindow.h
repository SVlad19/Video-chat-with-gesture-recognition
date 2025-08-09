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

    void HandleConnectionData(const QString& ipaddress);

    void on_actionExit_triggered();

    void on_actionDisconect_triggered();

private:
    void SetupClient();

    Ui::MainWindow *ui;
    QScopedPointer<class ConnectionWidget> ConnectionWindowWidget;
    QScopedPointer<class ClientManager> Client;
};
#endif // MAINWINDOW_H

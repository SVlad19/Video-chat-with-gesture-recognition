#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>

namespace Ui {
class ConnectionWidget;
}

class ConnectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionWidget(QWidget *parent = nullptr);
    ~ConnectionWidget();

signals:
    void ConnectionDataReady(const QString& ipaddress);

private slots:
    void on_btnConnect_clicked();

    void on_btnClose_clicked();

    void on_leIP_textChanged(const QString &arg1);

private:
    Ui::ConnectionWidget *ui;
    QString IPAddress;
};

#endif // CONNECTIONWIDGET_H

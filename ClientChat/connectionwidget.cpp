#include "connectionwidget.h"
#include "ui_connectionwidget.h"

ConnectionWidget::ConnectionWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectionWidget)
{
    ui->setupUi(this);
    ui->btnConnect->setEnabled(false);
    setWindowTitle("Connection");
}

ConnectionWidget::~ConnectionWidget()
{
    delete ui;
}

void ConnectionWidget::on_btnConnect_clicked()
{
    IPAddress = ui->leIP->text().trimmed();

    emit ConnectionDataReady(IPAddress);
    close();
}

void ConnectionWidget::on_btnClose_clicked()
{
    close();
}

void ConnectionWidget::on_leIP_textChanged(const QString &arg1)
{
     ui->leIP->text().trimmed().length() > 0 ? ui->btnConnect->setEnabled(true): ui->btnConnect->setEnabled(false);
}

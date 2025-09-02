#include "chatitemwidget.h"
#include "ui_chatitemwidget.h"

#include <QDateTime>

ChatItemWidget::ChatItemWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatItemWidget)
{
    ui->setupUi(this);
}

ChatItemWidget::~ChatItemWidget()
{
    delete ui;
}

void ChatItemWidget::SetMessage(const QString &Message, bool bIsMyMessage)
{
    if(bIsMyMessage){
        ui->lMessage->setAlignment(Qt::AlignRight);
    }

    ui->lMessage->setText(Message);
    ui->lTime->setText(QDateTime::currentDateTime().toString("HH:mm"));
}

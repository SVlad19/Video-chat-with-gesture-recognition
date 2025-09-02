#ifndef CHATITEMWIDGET_H
#define CHATITEMWIDGET_H

#include <QWidget>

namespace Ui {
class ChatItemWidget;
}

class ChatItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatItemWidget(QWidget *parent = nullptr);
    ~ChatItemWidget();
    void SetMessage(const QString& Message, bool bIsMyMessage = false);

private:
    Ui::ChatItemWidget *ui;
};

#endif // CHATITEMWIDGET_H

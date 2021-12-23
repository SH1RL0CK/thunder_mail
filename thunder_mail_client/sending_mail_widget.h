#ifndef SENDING_MAIL_WIDGET_H
#define SENDING_MAIL_WIDGET_H

#include <QWidget>

namespace Ui {
class SendingMailWidget;
}

class SendingMailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SendingMailWidget(QWidget *parent = nullptr);
    ~SendingMailWidget();

private:
    Ui::SendingMailWidget *ui;
};

#endif // SENDING_MAIL_WIDGET_H

#include "sending_mail_widget.h"
#include "ui_sending_mail_widget.h"

SendingMailWidget::SendingMailWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendingMailWidget)
{
    ui->setupUi(this);
}

SendingMailWidget::~SendingMailWidget()
{
    delete ui;
}

#include "menu_widget.h"
#include "ui_menu_widget.h"

MenuWidget::MenuWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MenuWidget)
{
    ui->setupUi(this);
    smtpClient = new SmtpClient(this);
    smtpClient->connectToServer("localhost", 4711);
}

MenuWidget::~MenuWidget()
{
    delete ui;
}


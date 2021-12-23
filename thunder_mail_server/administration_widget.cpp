#include "administration_widget.h"
#include "ui_administration_widget.h"


AdministrationWidget::AdministrationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdministrationWidget)
{
    ui->setupUi(this);
    smtpServer->startServer(4711);
}

AdministrationWidget::~AdministrationWidget()
{
    delete ui;
}


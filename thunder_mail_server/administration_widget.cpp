#include "administration_widget.h"
#include "ui_administration_widget.h"
#include "database_manager.h"


AdministrationWidget::AdministrationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdministrationWidget)
    , databaseManager(new DatabaseManager)
    , pop3Server(new Pop3Server(databaseManager))
    , smtpServer(new SmtpServer(databaseManager))
{
    ui->setupUi(this);
    pop3Server->startServer(4711);
    smtpServer->startServer(4712);
}

AdministrationWidget::~AdministrationWidget()
{
    delete ui;
}


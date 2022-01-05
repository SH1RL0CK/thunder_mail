#include "administration_widget.h"
#include "ui_administration_widget.h"
#include "database_manager.h"


AdministrationWidget::AdministrationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdministrationWidget)
    , databaseManager(new DatabaseManager)
    , smtpServer(new SmtpServer(databaseManager))
{
    ui->setupUi(this);
    databaseManager->delteteMail(1, 1);
    smtpServer->startServer(4711);
}

AdministrationWidget::~AdministrationWidget()
{
    delete ui;
}


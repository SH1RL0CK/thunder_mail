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
    QObject::connect(pop3Server, &Pop3Server::connectEvent, this, &AdministrationWidget::pop3ConnectEvent);
    QObject::connect(smtpServer, &SmtpServer::connectEvent, this, &AdministrationWidget::smtpConnectEvent);
    QObject::connect(pop3Server, &Pop3Server::newMessage, this, &AdministrationWidget::pop3Message);
    QObject::connect(smtpServer, &SmtpServer::newMessage, this, &AdministrationWidget::smtpMessage);
}

AdministrationWidget::~AdministrationWidget()
{
    delete ui;
}


void AdministrationWidget::on_startServerPushButton_clicked()
{
    int pop3Port = ui->pop3PortSpinBox->value();
    int smtpPort = ui->smtpPortSpinBox->value();
    if(pop3Port == smtpPort)
    {
        return;
    }
    pop3Server->startServer(pop3Port);
    smtpServer->startServer(smtpPort);
    ui->pop3PortSpinBox->setDisabled(true);
    ui->smtpPortSpinBox->setDisabled(true);
    ui->startServerPushButton->setDisabled(true);
    pop3LogMessage("Der Server wurde gestartet.");
    smtpLogMessage("Der Server wurde gestartet.");
}

void AdministrationWidget::pop3ConnectEvent(QString message, int numberOfClients)
{
    ui->pop3ClientsLabel->setText("Verbundene Clients: " + QString::number(numberOfClients));
    pop3LogMessage(message);
}

void AdministrationWidget::smtpConnectEvent(QString message, int numberOfClients)
{
    ui->smtpClientsLabel->setText("Verbundene Clients: " + QString::number(numberOfClients));
    smtpLogMessage(message);
}


void AdministrationWidget::pop3Message(QString sender, QString message)
{
    pop3LogMessage(sender + ": " + message);
}

void AdministrationWidget::smtpMessage(QString sender, QString message)
{
    smtpLogMessage(sender + ": " + message);
}

void AdministrationWidget::pop3LogMessage(QString message)
{
    ui->pop3LogListWidget->addItem(message);
    ui->pop3LogListWidget->scrollToBottom();
}

void AdministrationWidget::smtpLogMessage(QString message)
{
    ui->smtpLogListWidget->addItem(message);
    ui->smtpLogListWidget->scrollToBottom();
}

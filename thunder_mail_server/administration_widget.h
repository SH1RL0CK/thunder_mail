#ifndef ADMINISTRATIONWIDGET_H
#define ADMINISTRATIONWIDGET_H

#include <QWidget>

#include "pop3_server.h"
#include "smtp_server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AdministrationWidget; }
QT_END_NAMESPACE

class AdministrationWidget : public QWidget
{
    Q_OBJECT

public:
    AdministrationWidget(QWidget *parent = nullptr);
    ~AdministrationWidget();

private slots:
    void on_startServerPushButton_clicked();

    void pop3ConnectEvent(QString message, int numberOfClients);
    void smtpConnectEvent(QString message, int numberOfClients);

    void pop3Message(QString sender, QString message);
    void smtpMessage(QString sender, QString message);

private:
    Ui::AdministrationWidget *ui;
    DatabaseManager *databaseManager;
    Pop3Server *pop3Server;
    SmtpServer *smtpServer;

    void pop3LogMessage(QString message);
    void smtpLogMessage(QString message);
};
#endif // ADMINISTRATIONWIDGET_H

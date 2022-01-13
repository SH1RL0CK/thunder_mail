#ifndef ADMINISTRATIONWIDGET_H
#define ADMINISTRATIONWIDGET_H

#include <QWidget>

#include "pop3_server.h"
#include "smtp_server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AdministrationWidget; }
QT_END_NAMESPACE

/**
 * @brief Fenster, in der der Server gestartet und der Verkehr überwacht werden kann
 */
class AdministrationWidget : public QWidget
{
    Q_OBJECT

public:
    AdministrationWidget(QWidget *parent = nullptr);
    ~AdministrationWidget();

private slots:
    /**
     * @brief POP3- und SMTP-Server werden gestartet
     */
    void on_startServerPushButton_clicked();
    /**
     * @brief Ein Client hat sich mit dem POP3-Server verbunden/Verbindung getrennt
     * @param message Die Nachricht im Log
     * @param numberOfClients Die neue Anzahl der Clients
     */
    void pop3ConnectEvent(QString message, int numberOfClients);
    /**
     * @brief Ein Client hat sich mit dem SMTP-Server verbunden/Verbindung getrennt
     * @param message Die Nachricht im Log
     * @param numberOfClients Die neue Anzahl der Clients
     */
    void smtpConnectEvent(QString message, int numberOfClients);
    /**
     * @brief Im POP3-Protokoll wurde eine Nachricht geschickt
     * @param sender Sender der Nachricht (Client oder Server)
     * @param message Die Nachricht
     */
    void pop3Message(QString sender, QString message);
    /**
     * @brief Im SMTP-Protokoll wurde eine Nachricht geschickt
     * @param sender Sender der Nachricht (Client oder Server)
     * @param message Die Nachricht
     */
    void smtpMessage(QString sender, QString message);

private:
    Ui::AdministrationWidget *ui;
    /**
     * @brief Verwaltung der Datenbank
     */
    DatabaseManager *databaseManager;
    /**
     * @brief Der POP3-Server
     */
    Pop3Server *pop3Server;
    /**
     * @brief Der SMTP-Server
     */
    SmtpServer *smtpServer;

    /**
     * @brief Zeigt eine Nachricht im POP3-Log
     * @param message Die Nachricht
     */
    void pop3LogMessage(QString message);
    /**
     * @brief Zeigt eine Nachricht im SMTP-Log
     * @param message Die Nachricht
     */
    void smtpLogMessage(QString message);
};
#endif // ADMINISTRATIONWIDGET_H

#ifndef SMTPSERVER_H
#define SMTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "database_manager.h"

/**
 * @brief Struktur zum Speichern der Mail, die gerade an den Server geschickt wird
 */
struct Mail
{
    QString sender = "";
    QStringList recipients;
    QString subject = "";
    QString content = "";
};

/**
 * @brief Die verschiedene Stati, in denen sich, die mit dem Server verbunden Clients gerade befinden können
 */
enum SmtpClientState
{
    SmtpConnectedButNotVerified,
    SmtpConnectedAndVerified,
    SmtpStartedSendingNewMail,
    SmtpSendedAtLeastOneMailRecipient,
    SmtpSendedRequestToSendMailContent,
    SmtpSendedMailContent,
    SmtpSendedQuitRequest,
};


/**
 * @brief Struktur für die mit dem Server verbundenen Clients
*/
struct SmtpClient
{
    QString name;
    QTcpSocket *tcpSocket;
    SmtpClientState state;
    Mail *currentMail;
};

class SmtpServer : public QObject
{
    Q_OBJECT
public:
    explicit SmtpServer(DatabaseManager *_databaseManager, QObject *parent = nullptr);
    /**
     * @brief Der Server wird gestartet
     * @param port  Der Port, auf dem der Server läuft
     */
    void startServer(unsigned int port);

private slots:
    /**
     * @brief Ein neuer Client verbindet sich mit dem Server
     */
    void newConntection();
    /**
     * @brief Der Server empfängt Text von einem der Clients und verarbeitet diesen
     */
    void receiveAndHandleText();

private:
    /**
     * @brief Der TCP Server, mit dem sich die Clients verbinden können
     */
    QTcpServer *tcpServer;
    /**
     * @brief Die mit dem Server verbunden SMTP Clients
     */
    QList<SmtpClient*> clients;
    /**
     * @brief Die Schnittstelle zur Datenbank
     */
    DatabaseManager *databaseManager;

    /**
     * @brief Schickt einen Text einen Client
     * @param client  Der Client, an den der Text geschickt wird
     * @param text  Der zur schickende Text
     */
    void sendText(SmtpClient *client, QString text);
};

#endif // SMTPSERVER_H

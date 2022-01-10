#ifndef POP3SERVER_H
#define POP3SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include "database_manager.h"

/**
 * @brief Die verschiedene Stati, in denen sich, die mit dem Server verbunden Clients gerade befinden können
 */
enum Pop3ClientState
{
    Pop3ConnectedButNotVerified,
    Pop3SendedUsername,
    Pop3Verified,
};

struct Pop3Client
{
    int userId;
    QTcpSocket *tcpSocket;
    Pop3ClientState state;
    QList<DatabaseMail> mails;
    QList<DatabaseMail> mailsToBeDeleted;
};

class Pop3Server : public QObject
{
    Q_OBJECT
public:
    explicit Pop3Server(DatabaseManager *_databaseManager, QObject *parent = nullptr);

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
     * @brief Die mit dem Server verbunden POP3 Clients
     */
    QList<Pop3Client*> clients;
    /**
     * @brief Die Schnittstelle zur Datenbank
     */
    DatabaseManager *databaseManager;

    /**
     * @brief Schickt einen Text einen Client
     * @param client  Der Client, an den der Text geschickt wird
     * @param text  Der zur schickende Text
     */
    void sendText(Pop3Client *client, QString text);
};

#endif // POP3SERVER_H

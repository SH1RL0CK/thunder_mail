#ifndef POP3CLEINT_H
#define POP3CLEINT_H

#include <QObject>
#include <QTcpSocket>

struct Pop3ClientMail
{
    QString sender;
    QString recipients;
    QString dateTime;
    QString subject;
    QString body;
    bool marktForDelete = false;
};

/**
 * @brief Die verschiedene Stati, in den sich der POP3-Client gerade befinden kann
 */
enum Pop3ClientState
{
    Pop3NotConnected,
    Pop3ConnectedToServer,
    Pop3SendedUsername,
    Pop3SendedPassword,
    Pop3VerifiedAtServer,
    Pop3SendedStatCommand,
    Pop3WaitingForMails,
    Pop3SendedDeleteRequest,
    Pop3SendedNoOperationCommand,
    Pop3SendedResetRequest,
    Pop3SendedQuitRequest,
};

class Pop3Client : public QObject
{
    Q_OBJECT
public:
    explicit Pop3Client(QObject *parent = nullptr);

    Pop3ClientState getState();
    QString getUsername();
    QList<Pop3ClientMail> getReceivedMails();
    /**
     * @brief Der POP3-Client verbindet sich mit dem POP3-Server
     * @param ipAdress Die IP-Adresse des POP3-Servers
     * @param port Der Port auf dem der POP3-Server läuft
     */
    void connectToServer(QString ipAdress, unsigned int port, QString _username, QString _password);

    void getAllMails();

    void deleteMail(int mailIndex);

    void reset();

    void sendNoOperationCommand();

    void quit();

signals:
    /**
     * @brief Der Client hat sich erfolgreich am Server verifiziert
     */
    void verifiedSuccessfully();

    void verificationFailed();

    void receivedAllMails();

     void quittedServer();

private slots:
    /**
     * @brief Der Client empfängt Text vom Server
     */
    void receiveText();

private:
    /**
     * @brief Der Socket, der mit dem Server verbunden ist
     */
    QTcpSocket *tcpSocket;
    /**
     * @brief Der Status, in dem sich der POP3-Client gerade befindet
     */
    Pop3ClientState state;

    /**
     * @brief Die Anmeldedaten am POP3-Server
     */
    QString username;
    QString password;

    int numberOfAllMails;

    int currentMailIndex;

    QList<int> mailsMarkedForDelete;

    QList<Pop3ClientMail> receivedMails;
    /**
     * @brief Schickt einen Text an der Server
     * @param text Der zur schickende Text
     */
    void sendText(QString text);
    /**
     * @brief Verarbeitet die Antworten, die der POP3-Client vom Server empfängt
     * @param receivedText Der empfangene Text
     */
    void handleReceivedText(QString receivedText);
    void getNextMail();
    Pop3ClientMail createPop3ClientMail(QString content);
};

#endif // POP3CLEINT_H

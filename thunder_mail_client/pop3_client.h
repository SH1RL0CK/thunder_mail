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

/**
 * @brief Diese Klasse stellt den POP3-Client dar
 */
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
    /**
     * @brief Leitet das Holen der Mails vom Server ein
     */
    void getAllMails();
    /**
     * @brief Makiert eine Mail zum Löschen
     * @param mailIndex Der Index der Mail
     */
    void deleteMail(int mailIndex);
    /**
     * @brief Verwirfft alle Änderungen
     */
    void reset();
    /**
     * @brief Pingt den Server an
     */
    void sendNoOperationCommand();
    /**
     * @brief Sendet die QUIT-Anfrage an den Server
     */
    void quit();

signals:
    /**
     * @brief Der Client hat sich erfolgreich am Server verifiziert
     */
    void verifiedSuccessfully();
    /**
     * @brief Die Anmeldung ist gescheidert
     */
    void verificationFailed();
    /**
     * @brief Alle Mails wurden geschickt
     */
    void receivedAllMails();
    /**
     * @brief Der Client hat eine Antwort auf seine QUIT-Anfrage erhalten
     */
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
    /**
     * @brief Die Anzahl aller Mails, die gerade vom Server geholt werden
     */
    int numberOfAllMails;
    /**
     * @brief Der aktuelle Mail-Index (der gerade vom Server geholt wird)
     */
    int currentMailIndex;
    /**
     * @brief Die Mails, die zum löschen makiert sind, dient zum Anzeigen in der GUI
     */
    QList<int> mailsMarkedForDelete;
    /**
     * @brief Alle empfangenen Mails
     */
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
    /**
     * @brief Holt sich die nächste Mail vom Server, bis alle geholt sind
     */
    void getNextMail();
    /**
     * @brief Erzeugt aus dem Inhalt einer Mail, ein Pop3ClientMail Objekt
     * @param content Der Inhalt
     * @return Das erzeugt Pop3ClientMail Objekt
     */
    Pop3ClientMail createPop3ClientMail(QString content);
};

#endif // POP3CLEINT_H

#ifndef SMTPCLIENT_H
#define SMTPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostInfo>
#include <QDateTime>

/**
 * @brief Die verschiedene Stati, in den sich der SMTP Client gerade befinden kann
 */
enum SmtpClientState
{
    NotConnected,
    ConnectedToServer,
    SendedVerifyRequest,
    VerifiedAtServer,
    StartedSendingNewMail,
    SendedRequestToSendMailContent,
    SendedMailContent,
    SendedQuitRequest,
    SendedNoOperationCommand,
    SendedResetRequest,
};

/**
 * @brief Struktur zum Speichern der Mail, die gerade verschickt wird
 */
struct Mail
{
    QString sender = "";
    QStringList recipients;
    QString content = "";
};

/**
 * @brief Diese Klasse ist für den SMTP Client zuständig
 */
class SmtpClient : public QObject
{
    Q_OBJECT
public:

    explicit SmtpClient(QObject *parent = nullptr);

    QString getLastestError();
    /**
     * @brief Der SMTP Client verbindet sich mit dem SMTP Server
     * @param ipAdress  Die IP-Adresse des SMTP Servers
     * @param port  Der Port auf dem der SMTP Server läuft
     */
    void connectToServer(QString ipAdress, unsigned int port);
    /**
     * @brief Leitet das Vershicken einer Mail an den Server ein
     * @param sender  Der Absender der Mail
     * @param recipients  Die Empänger der Mail
     * @param subject  Der Betreff der Mail
     * @param body  Der Inhalt der Mail
     */
    void sendMail(QString sender, QStringList recipients, QString subject, QString body);
    /**
     * @brief Sendet eine Anfrage an den Server, die Verbindung zu schließen
     */
    void sendQuitRequest();
    /**
     * @brief Sendet eine Anfrage an den Server, die eine Antwort vom Server bewirk. Dies wird benutzt, um Timeouts zu verhindern.
     */
    void sendNoOperationCommand();
    /**
     * @brief Sendet eine Anfrage an den Server, die aktuelle Mailübertragung abzubrechen.
     */
    void sendResetRequest();

signals:
    /**
     * @brief Das Verbinden zum Server, war erfolgreich.
     */
    void connectedToServerSuccessfully();
    /**
     * @brief Das Senden der E-Mail war erfolgreich.
     */
    void sendingMailWasSucessful();
    /**
     * @brief Bei Senden der Mail gab es einen Fehler
     */
    void sendingMaiFailed();

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
     * @brief Der Status, in dem sich der SMTP Client gerade befindet
     */
    SmtpClientState state;
    /**
     * @brief Die Mail, die gereade verschickt wird
     */
    Mail *currentMail;
    /**
     * @brief Der Letzte Fehler, der aufgetreten ist.
     */
    QString lastestError;

    /**
     * @brief Generiert eine neue Mail mit ihrem Inhalt
     * @param sender  Der Absender der Mail
     * @param recipients  Die Empänger der Mail
     * @param subject  Der Betreff der Mail
     * @param body  Der Inhalt der Mail
     */
    void generateNewMail(QString sender, QStringList recipients, QString subject, QString body);
    /**
     * @brief Schickt einen Text an der Server
     * @param text  Der zur schickende Text
     */
    void sendText(QString text);
    /**
     * @brief Verarbeitet die Antworten, die der SMTP Client vom Server empfängt
     * @param receivedText  Der empfangene Text
     */
    void handleReceivedText(QString receivedText);
};

#endif // SMTPCLIENT_H

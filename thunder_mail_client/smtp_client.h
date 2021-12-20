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
    ConnectedToServer,
    NotConnected,
    SendedVerifyRequest,
    VerifiedAtServer,
    StartedSendingNewMail,
    SendedMailRecipient,
    SendedRequestToSendMailContent,
    SendedMailContent,
    SendedQuitRequest,
};

/**
 * @brief Struktur zum Speichern der Mail, die gerade verschickt wird
 */
struct Mail
{
    QString sender = "";
    QString recipient = "";
    QString subject = "";
    QString body = "";
};

/**
 * @brief Diese Klasse ist für den SMTP Client zuständig
 */
class SmtpClient : public QObject
{
    Q_OBJECT
public:

    explicit SmtpClient(QObject *parent = nullptr);

    /**
     * @brief Der SMTP Client verbindet sich mit dem SMTP Server
     * @param ipAdress  Die IP-Adresse des SMTP Servers
     * @param port  Der Port auf dem der SMTP Server läuft
     */
    void connectToServer(QString ipAdress, int port);
    /**
     * @brief Leitet das Vershicken einer Mail an den Server ein
     * @param sender  Der Absender der Mail
     * @param recipient  Der Empfänger der Mail
     * @param subject  Der Betreff der Mail
     * @param body  Der Inhalt der Mail
     */
    void sendMail(QString sender, QString recipient, QString subject, QString body);

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
     * @brief Der Status in dem sich der SMTP Client gerade befindet
     */
    SmtpClientState state;
    /**
     * @brief Die Mail, die gereade verschickt wird
     */
    Mail *currentMail;

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

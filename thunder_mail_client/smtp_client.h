#ifndef SMPTCLIENT_H
#define SMPTCLIENT_H

#include <QObject>
#include <QTcpSocket>

/**
 * @brief Die verschiedene Stati, in den sich der SMPT Client gerade befinden kann
 */
enum SmtpClientState
{
    ConnectedToServer,
    Smtp
};

/**
 * @brief Diese Klasse ist für den SMPT Client zuständig
 */
class SmtpClient : public QObject
{
    Q_OBJECT
public:
    explicit SmtpClient(QObject *parent = nullptr);

    /**
     * @brief Der SMPT Client verbindet sich mit dem SMPT Server
     * @param ipAdress  Die IP-Adresse des SMPT Servers
     * @param port  Der Port auf dem der SMPT Server läuft
     */
    void connectToServer(QString ipAdress, int port);

private slots:
    /**
     * @brief Der Client empfängt Text vom Server
     */
    void receiveText();

private:
    /**
     * @brief  Der Socket, der mit dem Server verbunden ist
     */
    QTcpSocket *tcpSocket;

    /**
     * @brief Schickt einen Text an der Server
     * @param text  Der zur schickende Text
     */
    void sendText(QString text);
    /**
     * @brief Verarbeitet die Antworten, die der SMPT Client vom Server empfängt
     * @param receivedText  Der empfangene Text
     */
    void handleReceivedText(QString receivedText);
};

#endif // SMPTCLIENT_H

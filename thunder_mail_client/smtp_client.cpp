#include "smtp_client.h"

SmtpClient::SmtpClient(QObject *parent)
    : QObject(parent)
    , tcpSocket(new QTcpSocket(this))
    , state(SmtpClientState::SmtpNotConnected)
    , currentMail(nullptr)
{}

QString SmtpClient::getLastestError()
{
    return lastestError;
}

SmtpClientState SmtpClient::getState()
{
    return state;
}

void SmtpClient::connectToServer(QString ipAdress, unsigned int port)
{
    if(state == SmtpClientState::SmtpNotConnected)
    {
        tcpSocket->connectToHost(ipAdress, port);
        state = SmtpClientState::SmtpConnectedToServer;
        QObject::connect(tcpSocket, &QTcpSocket::readyRead, this, &SmtpClient::receiveText);
    }
}

void SmtpClient::sendMail(QString sender, QStringList recipients, QString subject, QString body)
{
    generateNewMail(sender, recipients, subject, body);
    // Der Mailabsender wird an den Server geschickt und die Übertragung einer neuen Mail eingeleitet.
    sendText("MAIL FROM:" + currentMail->sender);
    state = SmtpClientState::SmtpStartedSendingNewMail;
}

void SmtpClient::sendQuitRequest()
{
    if(state != SmtpClientState::SmtpNotConnected)
    {
        sendText("QUIT");
        state = SmtpClientState::SmtpSendedQuitRequest;
    }
}

void SmtpClient::sendNoOperationCommand()
{
    if(state != SmtpClientState::SmtpNotConnected && state != SmtpClientState::SmtpConnectedToServer)
    {
        sendText("NOOP");
        state = SmtpClientState::SmtpSendedNoOperationCommand;
    }
}

void SmtpClient::sendResetRequest()
{
    if(state != SmtpClientState::SmtpNotConnected && state != SmtpClientState::SmtpConnectedToServer)
    {
        sendText("RSET");
        state = SmtpClientState::SmtpSendedResetRequest;
    }
}

void SmtpClient::receiveText()
{
    if(tcpSocket->bytesAvailable())
    {
        QString receivedText = tcpSocket->readAll();
        qDebug() << "received: " << receivedText;
        handleReceivedText(receivedText);
    }
}

void SmtpClient::generateNewMail(QString sender, QStringList recipients, QString subject, QString body)
{
    currentMail = new SmtpClientMail;
    // Absender- und Empfängeradressen werden zwischen "<" und ">" gesetzt.
    currentMail->sender = "<" + sender + ">";
    for(int i = 0; i < recipients.size(); i++)
    {
        recipients[i] = "<" + recipients[i] + ">";
    }
    currentMail->recipients = recipients;
    currentMail->content = "";
    // Der Mailheader mit Absender, Empänger, Betreff und aktueller Zeit wird erstellt.
    currentMail->content  += "From: " + currentMail->sender + "\n";
    currentMail->content  += "To: " + currentMail->recipients.join(",") + "\n";
    currentMail->content  += "Subject: " + subject  + "\n";
    currentMail->content  += "Date: " + QDateTime::currentDateTime().toString("ddd, dd MMM yyyy hh:mm:ss") + "\n\r\n";
    // Dann wird der eigentliche Mailinhalt hinzugefügt.
    currentMail->content  += body + "\n.";
}

void SmtpClient::sendText(QString text)
{
    if(state != SmtpClientState::SmtpNotConnected)
    {
        tcpSocket->write(text.toLatin1());
        qDebug() << "send: " << text;
    }
}

void SmtpClient::handleReceivedText(QString receivedText)
{
    int responseCode = receivedText.split(" ").at(0).toInt();
    /*
         Der Client hat sich mit dem Server verbunden und erhält eine Antwort von ihm,
         darauf hin sendet er eine Anfrage zur Verifizierung am Server.
    */
    if(responseCode == 220 && state == SmtpClientState::SmtpConnectedToServer)
    {
        QString clientName = QHostInfo::localHostName();
        sendText("HELO " + clientName);
        state = SmtpClientState::SmtpSendedVerifyRequest;
    }
    else if(responseCode == 250)
    {
        switch (state)
        {
            // Der Client hat eine Antwort auf seine Verifizierungsanfrage erhalten.
            case SmtpClientState::SmtpSendedVerifyRequest:
                state = SmtpClientState::SmtpVerifiedAtServer;
                emit connectedToServerSuccessfully();
                break;
            // Der Client hat eine Antwort vom Server auf die Anfrage erhalten, jetzt eine neue Mail zu schicken.
            case SmtpClientState::SmtpStartedSendingNewMail:
                // Der Client schickt nach und nach alle Empänger der Mail.
                if(currentMail->recipients.size() > 0)
                {
                    sendText("RCPT TO:" + currentMail->recipients.first());
                    currentMail->recipients.removeFirst();
                }
                // Sind alle versendet, kündigt er den Mailinhalt zu schicken.
                else
                {
                    sendText("DATA");
                    state = SmtpClientState::SmtpSendedRequestToSendMailContent;
                }
                break;
            // Der Mailinhalt wurde vom Server akzeptiert. Die Mail wurde erfolgreich versand.
            case SmtpClientState::SmtpSendedMailContent:
                delete currentMail;
                currentMail = nullptr;
                state = SmtpClientState::SmtpVerifiedAtServer;
                emit sendingMailWasSucessful();
                break;
            // Der Mailversand wurde erfolgreich abgebrochen.
            case SmtpClientState::SmtpSendedResetRequest:
                delete currentMail;
                currentMail = nullptr;
                state = SmtpClientState::SmtpVerifiedAtServer;
                break;
             // Der Client hat eine Antwort auf den "No Operation"-Befehl erhalten.
            case SmtpClientState::SmtpSendedNoOperationCommand:
                state = SmtpClientState::SmtpVerifiedAtServer;
                break;
            default:
                break;
        }
    }
    // Der Client schickt den Mailinhalt.
    else if(responseCode == 354 && state == SmtpClientState::SmtpSendedRequestToSendMailContent)
    {
        sendText(currentMail->content);
        state = SmtpClientState::SmtpSendedMailContent;
    }
    // Der Client hat eine Antwort auf den "QUIT"-Befehl erhalten und trennt die Verbindung zum Server.
    else if (responseCode == 221)
    {
        tcpSocket->disconnectFromHost();
        state = SmtpClientState::SmtpNotConnected;
        emit quittedServer();
    }
    else
    {
        lastestError = receivedText;
        emit sendingMaiFailed();
    }
}


#include "smtp_client.h"

SmtpClient::SmtpClient(QObject *parent)
    : QObject(parent)
    , tcpSocket(new QTcpSocket(this))
    , state(SmtpClientState::NotConnected)
    , currentMail(nullptr)
{}

QString SmtpClient::getLastestError()
{
    return lastestError;
}

void SmtpClient::connectToServer(QString ipAdress, unsigned int port)
{
    tcpSocket->connectToHost(ipAdress, port);
    state = SmtpClientState::ConnectedToServer;
    QObject::connect(tcpSocket, &QTcpSocket::readyRead, this, &SmtpClient::receiveText);
}

void SmtpClient::sendMail(QString sender, QStringList recipients, QString subject, QString body)
{
    generateNewMail(sender, recipients, subject, body);
    // Der Mailabsender wird an den Server geschickt und die Übertragung einer neuen Mail eingeleitet.
    sendText("MAIL FROM:" + currentMail->sender);
    state = SmtpClientState::StartedSendingNewMail;
}

void SmtpClient::sendQuitRequest()
{
    if(state != SmtpClientState::NotConnected)
    {
        sendText("QUIT");
        state = SmtpClientState::SendedQuitRequest;
    }
}

void SmtpClient::sendNoOperationCommand()
{
    if(state != SmtpClientState::NotConnected && state != SmtpClientState::ConnectedToServer)
    {
        sendText("NOOP");
        state = SmtpClientState::SendedNoOperationCommand;
    }
}

void SmtpClient::sendResetRequest()
{
    if(state != SmtpClientState::NotConnected && state != SmtpClientState::ConnectedToServer)
    {
        sendText("RSET");
        state = SmtpClientState::SendedResetRequest;
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
    currentMail = new Mail;
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
    currentMail->content  += "Date: " + QDateTime::currentDateTime().toString("ddd, dd MMM yyyy hh:mm:ss") + "\n";
    // Dann wird der eigentliche Mailinhalt hinzugefügt.
    currentMail->content  += body + "\n.";
}

void SmtpClient::sendText(QString text)
{
    if(state != SmtpClientState::NotConnected)
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
    if(responseCode == 220 && state == SmtpClientState::ConnectedToServer)
    {
        QString clientName = QHostInfo::localHostName();
        sendText("HELO " + clientName);
        state = SmtpClientState::SendedVerifyRequest;
    }
    else if(responseCode == 250)
    {
        switch (state)
        {
            // Der Client hat eine Antwort auf seine Verifizierungsanfrage erhalten.
            case SmtpClientState::SendedVerifyRequest:
                state = SmtpClientState::VerifiedAtServer;
                emit connectedToServerSuccessfully();
                break;
            // Der Client hat eine Antwort vom Server auf die Anfrage erhalten, jetzt eine neue Mail zu schicken.
            case SmtpClientState::StartedSendingNewMail:
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
                    state = SmtpClientState::SendedRequestToSendMailContent;
                }
                break;
            // Der Mailinhalt wurde vom Server akzeptiert. Die Mail wurde erfolgreich versand.
            case SmtpClientState::SendedMailContent:
                delete currentMail;
                currentMail = nullptr;
                state = SmtpClientState::VerifiedAtServer;
                emit sendingMailWasSucessful();
                break;
            // Der Mailversand wurde erfolgreich abgebrochen.
            case SmtpClientState::SendedResetRequest:
                delete currentMail;
                currentMail = nullptr;
                state = SmtpClientState::VerifiedAtServer;
                break;
             // Der Client hat eine Antwort auf den "No Operation"-Befehl erhalten.
            case SmtpClientState::SendedNoOperationCommand:
                state = SmtpClientState::VerifiedAtServer;
                break;
            default:
                break;
        }
    }
    // Der Client schickt den Mailinhalt.
    else if(responseCode == 354 && state == SmtpClientState::SendedRequestToSendMailContent)
    {
        sendText(currentMail->content);
        state = SmtpClientState::SendedMailContent;
    }
    // Der Client hat eine Antwort auf den "QUIT"-Befehl erhalten und trennt die Verbindung zum Server.
    else if (responseCode == 221)
    {
        tcpSocket->disconnectFromHost();
        state = SmtpClientState::NotConnected;
    }
    else
    {
        lastestError = receivedText;
        emit sendingMaiFailed();
    }
}


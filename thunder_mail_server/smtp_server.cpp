#include "smtp_server.h"

SmtpServer::SmtpServer(DatabaseManager *_databaseManager, QObject *parent)
    : QObject(parent)
    , tcpServer(new QTcpServer)
    , databaseManager(_databaseManager)
{}

void SmtpServer::startServer(unsigned int port)
{
    tcpServer->listen(QHostAddress::Any, port);
    QObject::connect(tcpServer, &QTcpServer::newConnection, this, &SmtpServer::newConntection);
}

void SmtpServer::newConntection()
{
    SmtpClient *newClient = new SmtpClient;
    newClient->name = "";
    newClient->tcpSocket = tcpServer->nextPendingConnection();
    newClient->state = SmtpClientState::SmtpConnectedButNotVerified;
    QObject::connect(newClient->tcpSocket, &QTcpSocket::readyRead, this, &SmtpServer::receiveAndHandleText);
    clients.append(newClient);
    sendText(newClient, "220 Service ready");
}

void SmtpServer::receiveAndHandleText()
{
    // Weil ich beim Durchlaufen der Liste Elemente auch eventuell lösche, nutze ich einen Iterator.
    QMutableListIterator<SmtpClient*> clientsIterator(clients);
    while(clientsIterator.hasNext())
    {
        SmtpClient *currentClient = clientsIterator.next();
        if(!currentClient->tcpSocket->bytesAvailable())
            continue;
        QString response = "";
        QString receivedText = currentClient->tcpSocket->readAll();
        // Der Client schickt den Mailinhalt.
        if(currentClient->state == SmtpClientState::SmtpSendedRequestToSendMailContent)
        {
            if(receivedText.endsWith("\n."))
            {
                response = "250 OK";
                receivedText.chop(2);
                currentClient->currentMail->content = receivedText;
                databaseManager->storeMail(currentClient->currentMail->recipients, currentClient->currentMail->content);
                currentClient->state = SmtpClientState::SmtpConnectedAndVerified;
            }
            else
                response = "521 Machine does not accpet mail";
        }
        // Der Client hat eine Verifizierungsanfrage an den Server gestellt.
        else if(receivedText.startsWith("HELO "))
        {
            if(currentClient->state == SmtpClientState::SmtpConnectedButNotVerified)
            {
                currentClient->name = receivedText.split(" ").at(1);
                currentClient->state = SmtpClientState::SmtpConnectedAndVerified;
                response = "250 OK";
            }
            else
                response = "503 Bad sequence of commands";
        }
        // Der Client hat möchte eine neue Mail senden.
        else if(receivedText.startsWith("MAIL FROM:"))
        {
            if(currentClient->state == SmtpClientState::SmtpConnectedAndVerified)
            {
                currentClient->currentMail = new Mail;
                currentClient->currentMail->sender = receivedText.split(":").at(1);
                currentClient->state = SmtpClientState::SmtpStartedSendingNewMail;
                response = "250 OK";
            }
            else
                response = "503 Bad sequence of commands";
        }
        // Der Client schickt den/die Empfänger der Mail.
        else if(receivedText.startsWith("RCPT TO:"))
        {
            if(currentClient->state == SmtpClientState::SmtpStartedSendingNewMail || currentClient->state == SmtpClientState::SmtpSendedAtLeastOneMailRecipient)
            {
                currentClient->currentMail->recipients.append(receivedText.split(":").at(1));
                currentClient->state = SmtpClientState::SmtpSendedAtLeastOneMailRecipient;
                response = "250 OK";
            }
            else
                response = "503 Bad sequence of commands";
        }
        // Der Client möchte den Mailinhalt übertragen.
        else if(receivedText == "DATA")
        {
            if(currentClient->state == SmtpClientState::SmtpSendedAtLeastOneMailRecipient)
            {
                currentClient->state = SmtpClientState::SmtpSendedRequestToSendMailContent;
                response = "354 Start mail input";
            }
            else
                response = "503 Bad sequence of commands";
        }
         // Der Client möchte die aktuelle Mailübertragung abbrechen.
        else if(receivedText == "RSET")
        {
            if(currentClient->state != SmtpClientState::SmtpConnectedButNotVerified){
                currentClient->state = SmtpClientState::SmtpConnectedAndVerified;
                delete currentClient->currentMail;
                currentClient->currentMail = nullptr;
                response = "250 OK";
            }
            else
                response = "503 Bad sequence of commands";
        }
        // Der Client möchte eine einfache Antwort vom Server.
        else if(receivedText == "NOOP")
        {
            sendText(currentClient, "250 OK");
        }
        /*
            Der Client will die Verbindung zum Server beenden. Darauf hin schickt der Server eine letzte Antwort und löscht
            den Client aus der Liste mit den Clients.
        */
        else if(receivedText == "QUIT")
        {
            sendText(currentClient, "221 Closing channel");
            delete currentClient;
            clientsIterator.remove();
            continue;
        }
        // Der Befehl wurde nicht gefunden.
        else
        {
            response = "500 Command not regonized: " + receivedText + ". Syntax error!";
        }
        sendText(currentClient, response);
    }
}

void SmtpServer::sendText(SmtpClient *client, QString text)
{
    client->tcpSocket->write(text.toLatin1());
    qDebug() << "send: " << text;
}

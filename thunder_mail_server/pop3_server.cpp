#include "pop3_server.h"

Pop3Server::Pop3Server(DatabaseManager *_databaseManager, QObject *parent)
: QObject(parent)
, tcpServer(new QTcpServer)
, databaseManager(_databaseManager)
{}

void Pop3Server::startServer(unsigned int port)
{
    tcpServer->listen(QHostAddress::Any, port);
    QObject::connect(tcpServer, &QTcpServer::newConnection, this, &Pop3Server::newConntection);
}

void Pop3Server::newConntection()
{
    Pop3Client *newClient = new Pop3Client;
    newClient->tcpSocket = tcpServer->nextPendingConnection();
    newClient->state = Pop3ClientState::Pop3ConnectedButNotVerified;
    QObject::connect(newClient->tcpSocket, &QTcpSocket::readyRead, this, &Pop3Server::receiveAndHandleText);
    clients.append(newClient);
    emit connectEvent("Ein neuer Client hat sich verbunden.", clients.size());
    sendText(newClient, "+OK");
}

void Pop3Server::receiveAndHandleText()
{
    // Weil ich beim Durchlaufen der Liste Elemente auch eventuell lösche, nutze ich einen Iterator.
    QMutableListIterator<Pop3Client*> clientsIterator(clients);
    while(clientsIterator.hasNext())
    {
        Pop3Client *currentClient = clientsIterator.next();
        if(!currentClient->tcpSocket->bytesAvailable())
            continue;
        QString response = "";
        QString receivedText = currentClient->tcpSocket->readAll();
        emit newMessage(currentClient->username, receivedText);
        if(receivedText.startsWith("USER ") && currentClient->state == Pop3ClientState::Pop3ConnectedButNotVerified)
        {
            currentClient->username = "<" + receivedText.split(" ").at(1) + ">";
            currentClient->userId = databaseManager->userExists(currentClient->username);
            if(currentClient->userId != -1)
            {
                currentClient->state = Pop3ClientState::Pop3SendedUsername;
                response = "+OK please enter password";
            }
            else
                response = "+ERR user does not exist";
        }
        else if(receivedText.startsWith("PASS ") && currentClient->state == Pop3ClientState::Pop3SendedUsername)
        {
            QString password = receivedText.split(" ").at(1);
            if(databaseManager->loginDataIsCorrect(currentClient->userId, password))
            {
                currentClient->state = Pop3ClientState::Pop3Verified;
                response = "+OK mailbox ready";
            }
            else
            {
                currentClient->state = Pop3ClientState::Pop3ConnectedButNotVerified;
                response = "+ERR wrong password";
            }
        }
        else if (currentClient->state == Pop3ClientState::Pop3Verified)
        {
            // Liefert die Anzahl und Größe aller Mails
            if(receivedText == "STAT")
            {
                currentClient->mails = databaseManager->getUsersMails(currentClient->userId);
                int mailsSize = 0;
                for(int i = 0; i < currentClient->mails.size(); i++)
                {
                    mailsSize += currentClient->mails.at(i).mailContent.toUtf8().size();
                }
                response = "+OK " + QString::number(currentClient->mails.size()) + " " + QString::number(mailsSize);
            }
            // Liefert die Größe der Mails einzeln
            else if(receivedText.startsWith("LIST"))
            {
                int limit = -1;
                if(receivedText.split(" ").size() > 1)
                    limit = receivedText.split(" ").at(1).toInt();
                currentClient->mails = databaseManager->getUsersMails(currentClient->userId, limit);
                response = "";
                int mailsSize = 0;
                for(int i = 0; i < currentClient->mails.size(); i++)
                {
                    int size = currentClient->mails.at(i).mailContent.toUtf8().size();
                    mailsSize += size;
                    response += QString::number(i+1) + " " + QString::number(size) + "\n";
                }
                response = "+OK mailbox has " + QString::number(currentClient->mails.size()) + " mail (" + QString::number(mailsSize) + " octets)\n" + response + "\n.";
            }
            // Liefert den Inhalt einer Mail
            else if(receivedText.startsWith("RETR "))
            {
                int indexInList = receivedText.split(" ").at(1).toInt() -1;
                if(indexInList >= 0 && indexInList < currentClient->mails.size()){
                    response = "+OK mail follows\n" + currentClient->mails.at(indexInList).mailContent + "\n.";
                }
                else
                    response = "+ERR mail does not exist";
            }
            // Makiert eine Mail als gelöscht
            else if(receivedText.startsWith("DELE "))
            {
                int indexInList = receivedText.split(" ").at(1).toInt() -1;
                if(indexInList >= 0 && indexInList < currentClient->mails.size()){
                    currentClient->mailsToBeDeleted.append(currentClient->mails.at(indexInList));
                    response = "+OK mail marked for delete";
                }
                else
                    response = "+ERR mail does not exist";
            }
            // Gibt einfach eine Antwoer
            else if(receivedText == "NOOP")
            {
                response = "+OK";
            }
            // Setzt alle Löschungen zurück
            else if(receivedText == "RSET")
            {
                currentClient->mailsToBeDeleted.clear();
                response = "+OK everything reseted";
            }
            // Trennt die Verbindung und führt alle Löschungen durch
            else if(receivedText == "QUIT")
            {
                for(int i = 0; i < currentClient->mailsToBeDeleted.size(); i++)
                {
                    databaseManager->delteteMail(currentClient->userId, currentClient->mailsToBeDeleted.at(i).mailId);
                }
                sendText(currentClient, "+OK bye");
                currentClient->tcpSocket->disconnectFromHost();
                clientsIterator.remove();
                emit connectEvent(currentClient->username + " hat die Verbindung getrennt.", clients.size());
                delete currentClient;
                continue;
            }
        }
        if(response == "")
        {
            response = "+ERR unkown command";
        }
        sendText(currentClient, response);
    }

}

void Pop3Server::sendText(Pop3Client *client, QString text)
{
    client->tcpSocket->write(text.toLatin1());
    newMessage("Server", text);
}

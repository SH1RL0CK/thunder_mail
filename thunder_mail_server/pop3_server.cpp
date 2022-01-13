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
        qDebug() << receivedText;
        if(receivedText.startsWith("USER ") && currentClient->state == Pop3ClientState::Pop3ConnectedButNotVerified)
        {
            currentClient->userId = databaseManager->userExists("<" + receivedText.split(" ").at(1) + ">");
            qDebug() << currentClient->userId;
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
            else if(receivedText.startsWith("LIST"))
            {
                currentClient->mails = databaseManager->getUsersMails(currentClient->userId);
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
            else if(receivedText.startsWith("RETR "))
            {
                int indexInList = receivedText.split(" ").at(1).toInt() -1;
                if(indexInList >= 0 && indexInList < currentClient->mails.size()){
                    response = "+OK mail follows\n" + currentClient->mails.at(indexInList).mailContent + "\n.";
                }
                else
                    response = "+ERR mail does not exist";
            }
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
            else if(receivedText == "NOOP")
            {
                response = "+OK";
            }
            else if(receivedText == "RSET")
            {
                currentClient->mailsToBeDeleted.clear();
                response = "+OK everything reseted";
            }
            else if(receivedText == "QUIT")
            {
                for(int i = 0; i < currentClient->mailsToBeDeleted.size(); i++)
                {
                    databaseManager->delteteMail(currentClient->userId, currentClient->mailsToBeDeleted.at(i).mailId);
                }
                sendText(currentClient, "+OK bye");
                delete currentClient;
                clientsIterator.remove();
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
    qDebug() << "send: " << text;
}

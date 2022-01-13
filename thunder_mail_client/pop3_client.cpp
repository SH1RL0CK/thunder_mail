#include "pop3_client.h"

Pop3Client::Pop3Client(QObject *parent)
    : QObject(parent)
    , tcpSocket(new QTcpSocket)
    , state(Pop3ClientState::Pop3NotConnected)
    , numberOfAllMails(0)
    , currentMailIndex(0)
{
    QObject::connect(tcpSocket, &QTcpSocket::readyRead, this, &Pop3Client::receiveText);
}

Pop3ClientState Pop3Client::getState()
{
    return state;
}

QString Pop3Client::getUsername()
{
    return username;
}

QList<Pop3ClientMail> Pop3Client::getReceivedMails()
{
    return receivedMails;
}


void Pop3Client::connectToServer(QString ipAdress, unsigned int port, QString _username, QString _password)
{
    username = _username;
    password = _password;
    //Falls die Methode zum ersten Mal aufgerufen wurde, verbindet er sich neu mit dem Server
    if(state == Pop3ClientState::Pop3NotConnected)
    {
        tcpSocket->connectToHost(ipAdress, port);
        state = Pop3ClientState::Pop3ConnectedToServer;
    }
    //Falls der Client bereits verbunden ist und nur nochmal neue Anmeldedaten ausprobiert
    else if(state == Pop3ClientState::Pop3ConnectedToServer)
    {
        sendText("USER " + username);
        state = Pop3ClientState::Pop3SendedUsername;
    }
}

void Pop3Client::getAllMails()
{
    //Schickt zunächst den STAT-Befehl um die Anzahl der Mails auf dem Server zu erhalten
    sendText("STAT");
    state = Pop3ClientState::Pop3SendedStatCommand;
}

void Pop3Client::deleteMail(int mailIndex)
{
    sendText("DELE " + QString::number(mailIndex));
    mailsMarkedForDelete.append(mailIndex);
    state = Pop3ClientState::Pop3SendedDeleteRequest;
}

void Pop3Client::reset()
{
    sendText("RSET");
    state = Pop3ClientState::Pop3SendedResetRequest;
}

void Pop3Client::sendNoOperationCommand()
{
    sendText("NOOP");
    state = Pop3ClientState::Pop3SendedNoOperationCommand;
}

void Pop3Client::quit()
{
    sendText("QUIT");
    state = Pop3ClientState::Pop3SendedQuitRequest;
}

void Pop3Client::receiveText()
{
    if(tcpSocket->bytesAvailable())
    {
        QString receivedText = tcpSocket->readAll();
        handleReceivedText(receivedText);
    }
}

void Pop3Client::sendText(QString text)
{
    if(state != Pop3ClientState::Pop3NotConnected)
    {
        tcpSocket->write(text.toLatin1());
    }
}

void Pop3Client::handleReceivedText(QString receivedText)
{
    QString responseStatus = receivedText.split(" ").at(0);
    Pop3ClientMail mail;
    if(responseStatus == "+OK")
    {
        switch(state)
        {
            case Pop3ClientState::Pop3ConnectedToServer:
                sendText("USER " + username);
                state = Pop3ClientState::Pop3SendedUsername;
                break;
            case Pop3ClientState::Pop3SendedUsername:
                sendText("PASS " + password);
                state = Pop3ClientState::Pop3SendedPassword;
                break;
            case Pop3ClientState::Pop3SendedPassword:
                state = Pop3ClientState::Pop3VerifiedAtServer;
                emit verifiedSuccessfully();
                break;
            case Pop3ClientState::Pop3SendedStatCommand:
                //Nun weiß der Client die Anzahl aller Mails, die er ehält
                numberOfAllMails = receivedText.split(" ").at(1).toInt();
                receivedMails.clear();
                //Und holt sie nun nach und nach vom Server
                currentMailIndex = 0;
                state = Pop3ClientState::Pop3WaitingForMails;
                getNextMail();
                break;
            case Pop3ClientState::Pop3WaitingForMails:
                //Er empfängt den neuen Mailinhalt und versucht dann die nächste Mail abzuholen
                mail = createPop3ClientMail(receivedText.section("\n", 1));
                receivedMails.append(mail);
                getNextMail();
                break;
            case Pop3ClientState::Pop3SendedDeleteRequest:
                state = Pop3ClientState::Pop3VerifiedAtServer;
                //Holt erneut alle Mails vom Server, um die als makiert gelöscht makierten in der GUI anzeigen zu können
                getAllMails();
                break;
            case Pop3ClientState::Pop3SendedResetRequest:
                mailsMarkedForDelete.clear();
                state = Pop3ClientState::Pop3VerifiedAtServer;
                //Holt erneut alle Mails vom Server, um die nun nicht mehr als gelöscht makierten in der GUI anzeigen zu können
                getAllMails();
                break;
            case Pop3ClientState::Pop3SendedQuitRequest:
                tcpSocket->disconnectFromHost();
                state = Pop3ClientState::Pop3NotConnected;
                emit quittedServer();
                break;
            case Pop3ClientState::Pop3SendedNoOperationCommand:
                state = Pop3ClientState::Pop3VerifiedAtServer;
                break;
        }
    } else if(responseStatus == "+ERR")
    {
        switch(state)
        {
            //Fehler beim Anmelden
            case Pop3ClientState::Pop3SendedUsername:
            case Pop3ClientState::Pop3SendedPassword:
                state = Pop3ClientState::Pop3ConnectedToServer;
                emit verificationFailed();
                break;
        }
    }
}

void Pop3Client::getNextMail()
{
    currentMailIndex++;
    //Falls alle Mails abgeholt wurden
    if(currentMailIndex > numberOfAllMails)
    {
        state = Pop3ClientState::Pop3VerifiedAtServer;
        currentMailIndex = 0;
        numberOfAllMails = 0;
        emit receivedAllMails();
        return;
    }
    sendText("RETR " + QString::number(currentMailIndex));
}

Pop3ClientMail Pop3Client::createPop3ClientMail(QString content)
{
    Pop3ClientMail mail;
    QStringList header = content.section("\n\r\n", 0,0).split("\n");
    for(int i = 0; i < header.size(); i++)
    {
        //Liest alles aus dem Header aus
        if(header.at(i).startsWith("From: "))
        {
            mail.sender = header.at(i).section(" ", 1);
        }
        else if(header.at(i).startsWith("To: "))
        {
            mail.recipients = header.at(i).section(" ", 1);
        }
        else if(header.at(i).startsWith("Date: "))
        {
            mail.dateTime = header.at(i).section(" ", 1);
        }
        else if(header.at(i).startsWith("Subject: "))
        {
            mail.subject = header.at(i).section(" ", 1);
        }
    }
    //Und dann den Body
    mail.body = content.section("\n\r\n", 1);
    mail.body.chop(2);
    //Überprüft, ob die Mail zum Löschen makiert ist
    if(mailsMarkedForDelete.contains(currentMailIndex))
    {
        mail.marktForDelete = true;
    }
    return mail;
}

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
    tcpSocket->connectToHost(ipAdress, port);
    state = Pop3ClientState::Pop3ConnectedToServer;
    username = _username;
    password = _password;
}

void Pop3Client::getAllMails()
{
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

void Pop3Client::receiveText()
{
    if(tcpSocket->bytesAvailable())
    {
        QString receivedText = tcpSocket->readAll();
        qDebug() << "received: " << receivedText;
        handleReceivedText(receivedText);
    }
}

void Pop3Client::sendText(QString text)
{
    if(state != Pop3ClientState::Pop3NotConnected)
    {
        tcpSocket->write(text.toLatin1());
        qDebug() << "send: " << text;
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
                numberOfAllMails = receivedText.split(" ").at(1).toInt();
                receivedMails.clear();
                currentMailIndex = 0;
                state = Pop3ClientState::Pop3WaitingForMails;
                getNextMail();
                break;
            case Pop3ClientState::Pop3WaitingForMails:
                mail = createPop3ClientMail(receivedText.section("\n", 1));
                receivedMails.append(mail);
                getNextMail();
                break;
            case Pop3ClientState::Pop3SendedDeleteRequest:
                state = Pop3ClientState::Pop3VerifiedAtServer;
                getAllMails();
                break;
            case Pop3ClientState::Pop3SendedResetRequest:
                mailsMarkedForDelete.clear();
                state = Pop3ClientState::Pop3VerifiedAtServer;
                getAllMails();
                break;
        }
    }
}

void Pop3Client::getNextMail()
{
    currentMailIndex++;
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
    mail.body = content.section("\n\r\n", 1);
    mail.body.chop(2);
    if(mailsMarkedForDelete.contains(currentMailIndex))
    {
        mail.marktForDelete = true;
    }
    return mail;
}

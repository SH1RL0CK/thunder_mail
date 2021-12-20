#include "smtp_client.h"

SmtpClient::SmtpClient(QObject *parent)
    : QObject(parent)
    , tcpSocket(new QTcpSocket(this))
    , state(SmtpClientState::NotConnected)
    , currentMail(nullptr)
{}

void SmtpClient::connectToServer(QString ipAdress, int port)
{
    tcpSocket->connectToHost(ipAdress, port);
    QObject::connect(tcpSocket, &QTcpSocket::readyRead, this, &SmtpClient::receiveText);
    state = SmtpClientState::ConnectedToServer;
}

void SmtpClient::sendMail(QString sender, QString recipient, QString subject, QString body)
{
    currentMail = new Mail;
    currentMail->sender = "<" + sender + ">";
    currentMail->recipient = "<" + recipient + ">";
    currentMail->subject = subject;
    currentMail->body = body;
    sendText("MAIL FROM:" + currentMail->sender);
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

void SmtpClient::sendText(QString text)
{
    tcpSocket->write(text.toLatin1());
    qDebug() << "send: " << text;
}

void SmtpClient::handleReceivedText(QString receivedText)
{
    int responseCode = receivedText.split(" ").at(0).toInt();
    if(responseCode == 220 && state == SmtpClientState::ConnectedToServer)
    {
        QString clientName = QHostInfo::localHostName();
        sendText("HELO " + clientName);
    }
    else if(responseCode == 250)
    {
        switch (state)
        {
            case SmtpClientState::SendedVerifyRequest:
                state = SmtpClientState::VerifiedAtServer;
                break;
            case SmtpClientState::StartedSendingNewMail:
                sendText("RCPT TO:" + currentMail->recipient);
                state = SmtpClientState::SendedMailRecipient;
                break;
            case SmtpClientState::SendedMailRecipient:
                sendText("DATA");
                state = SmtpClientState::SendedRequestToSendMailContent;
                break;
            default:
                break;
        }
    }
    else if(responseCode == 354 && state == SendedRequestToSendMailContent)
    {
        QString mailContent = "";
        mailContent += "From: " + currentMail->sender + "\n";
        mailContent += "To: " + currentMail->recipient  + "\n";
        mailContent += "Subject: " + currentMail->subject  + "\n";
        mailContent += "Date: " + QDateTime::currentDateTime().toString("ddd, dd MMM yyyy hh:mm:ss");
        mailContent += currentMail->body + "\n.";
        sendText(mailContent);
        state = SmtpClientState::SendedMailContent;
    }
    else if (responseCode == 221)
    {
        tcpSocket->close();
        state = SmtpClientState::NotConnected;
    }
    else
    {
        qDebug() << "unexpected response!";
    }
}


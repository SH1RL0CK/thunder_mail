#include "smtp_client.h"

SmtpClient::SmtpClient(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
}

void SmtpClient::connectToServer(QString ipAdress, int port)
{
    tcpSocket->connectToHost(ipAdress, port);
    QObject::connect(tcpSocket, &QTcpSocket::readyRead, this, &SmtpClient::receiveText);
    qDebug() << "connected to server";
}

void SmtpClient::receiveText()
{
    if(tcpSocket->bytesAvailable())
    {
        QString receivedText = tcpSocket->readLine();
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
    qDebug() << "handle: " << receivedText;
}


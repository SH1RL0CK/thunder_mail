#include "smpt_client.h"

SmptClient::SmptClient(QObject *parent) : QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
}

void SmptClient::connectToServer(QString ipAdress, int port)
{
    tcpSocket->connectToHost(ipAdress, port);
    QObject::connect(tcpSocket, &QTcpSocket::readyRead, this, &SmptClient::receiveText);
    qDebug() << "connected to server";
}

void SmptClient::receiveText()
{
    if(tcpSocket->bytesAvailable())
    {
        QString receivedText = tcpSocket->readLine();
        qDebug() << "received: " << receivedText;
        handleReceivedText(receivedText);
    }
}

void SmptClient::sendText(QString text)
{
    tcpSocket->write(text.toLatin1());
    qDebug() << "send: " << text;
}

void SmptClient::handleReceivedText(QString receivedText)
{
    qDebug() << "handle: " << receivedText;
}


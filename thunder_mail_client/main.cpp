#include "sending_mail_widget.h"

#include <QApplication>

#include "smtp_client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SmtpClient *smtpCLient = new SmtpClient;
    smtpCLient->connectToServer("127.0.0.1", 4711);
    SendingMailWidget w(smtpCLient, "philipp@thundermail.de");
    w.show();
    return a.exec();
}

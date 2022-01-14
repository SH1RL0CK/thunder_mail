#include "sending_mail_widget.h"

#include <QApplication>

#include "login_widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWidget loginWidget;
    //SendingMailWidget w(smtpCLient, "philipp@thundermail.de");
    loginWidget.show();
    return a.exec();
}

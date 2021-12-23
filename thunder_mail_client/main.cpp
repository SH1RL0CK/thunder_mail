#include "sending_mail_widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SendingMailWidget w;
    w.show();
    return a.exec();
}

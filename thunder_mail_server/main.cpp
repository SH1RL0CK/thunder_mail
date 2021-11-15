#include "administration_widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AdministrationWidget w;
    w.show();
    return a.exec();
}

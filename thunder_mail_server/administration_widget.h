#ifndef ADMINISTRATIONWIDGET_H
#define ADMINISTRATIONWIDGET_H

#include "smtp_server.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class AdministrationWidget; }
QT_END_NAMESPACE

class AdministrationWidget : public QWidget
{
    Q_OBJECT

public:
    AdministrationWidget(QWidget *parent = nullptr);
    ~AdministrationWidget();

private:
    Ui::AdministrationWidget *ui;
    DatabaseManager *databaseManager;
    SmtpServer *smtpServer;
};
#endif // ADMINISTRATIONWIDGET_H

#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>
#include <QCloseEvent>

#include "pop3_client.h"
#include "smtp_client.h"
#include "sending_mail_widget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MenuWidget; }
QT_END_NAMESPACE

/**
 * @brief Das Menü, in dem der Benutzer seine Mailbox sieht und diese verwalten kann.
 */
class MenuWidget : public QWidget
{
    Q_OBJECT

public:
    MenuWidget(Pop3Client *_pop3Client, SmtpClient *_smtpClient, QWidget *parent = nullptr);
    ~MenuWidget();

private slots:

    void on_sendMailPushButton_clicked();

    void receivedAllMails();

    void quittedServer();

    void on_receiveMailsPushButton_clicked();


    void on_mailsTableWidget_cellClicked(int row, int column);

    void on_deleteMailPushButton_clicked();

    void on_resetPushButton_clicked();


private:
    Ui::MenuWidget *ui;
    Pop3Client *pop3Client;
    SmtpClient *smtpClient;
    int selectedMailIndex;

    void showMail(int mailIndex);

    void closeEvent(QCloseEvent *bar);

};
#endif // MENUWIDGET_H

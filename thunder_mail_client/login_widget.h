#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include <QWidget>

#include "pop3_client.h"
#include "smtp_client.h"
#include "menu_widget.h"


namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

private slots:
    void on_loginPushButton_clicked();
    void clientWasVerifiedSuccessfully();
    void loginFailed();

private:
    Ui::LoginWidget *ui;

    Pop3Client *pop3Client;
    SmtpClient *smtpClient;
};

#endif // LOGIN_WIDGET_H

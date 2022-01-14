#ifndef LOGIN_WIDGET_H
#define LOGIN_WIDGET_H

#include <QWidget>

#include "pop3_client.h"
#include "smtp_client.h"
#include "menu_widget.h"


namespace Ui {
class LoginWidget;
}

/**
 * @brief Das Fenster, in dem sich der Benutzer, mit dem POP3 und SMTP-Sever verbinden kann
 */
class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

private slots:
    /**
     * @brief Meldet sich am POP- und SMTP-Server an
     */
    void on_loginPushButton_clicked();
    /**
     * @brief Die Anmeldung war efolgreich, das Menü wird geöffnet
     */
    void clientWasVerifiedSuccessfully();
    /**
     * @brief Der Login ist gescheitert
     */
    void loginFailed();

private:
    Ui::LoginWidget *ui;

    /**
     * @brief Der POP-Client
     */
    Pop3Client *pop3Client;
    /**
     * @brief Der SMTP-Client
     */
    SmtpClient *smtpClient;
};

#endif // LOGIN_WIDGET_H

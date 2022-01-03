#ifndef SENDING_MAIL_WIDGET_H
#define SENDING_MAIL_WIDGET_H

#include <QWidget>
#include <QMessageBox>

#include "smtp_client.h"

namespace Ui {
class SendingMailWidget;
}

class SendingMailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SendingMailWidget(SmtpClient *_smtpClient, QString sender, QWidget *parent = nullptr);
    ~SendingMailWidget();

private slots:
    /**
     * @brief Der Benutzer hat den Knopf zum Absenden der Mail gedrückt.
     */
    void on_sendMailPushButton_clicked();
    /**
     * @brief Dieser Slot wird mit dem Signal des SMTP Clients verbunden und aufgerufen,
     * wenn das Versenden der Mail erfolgreich war. Der Benutzer wird mit Hilfe einer Nachricht darüber informiert.
     */
    void sendingMailWasSuccessful();
    /**
     * @brief Dieser Slot wird mit dem Signal des SMTP Clients verbunden und aufgerufen,
     * wenn das Versenden der Mail gescheitert ist. Der Benutzer wird mit Hilfe einer Nachricht darüber informiert.
     */
    void sendingMailFailed();

private:
    Ui::SendingMailWidget *ui;
    /**
     * @brief Ein Pointer auf den SMTP Client.
     */
    SmtpClient *smtpClient;

    /**
     * @brief Zeigt dem Benutzer ein entsprechend farbig betontes Feedback.
     * @param feedback  Die Nachricht, die angezeigt wird
     * @param color  Die Farbe, in der sie angezeigt wird
     */
    void showFeedbackOutput(QString feedback, QString color);
    /**
     * @brief Überpüft ob die angegebenen Mail-Adressen (Absender und Empfänger) wirklich Mailadressen sind.
     * @param sender  Der Absender
     * @param recipients  Der/Die Empänger
     * @return true, wenn die Mail-Adressen richtig sind, ansonsten false
     */
    bool mailsAreValid(QString sender, QStringList recipients);
    /**
     * @brief Setzt bei allen Eingabefelder, ob diese disabled sind oder nicht.
     * @param disabled
     */
    void setInputsDisabled(bool disabled);
};

#endif // SENDING_MAIL_WIDGET_H

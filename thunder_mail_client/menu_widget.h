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
    /**
     * @brief Der POP3-Client hat alle Mails erhalten, diese werden in einer Tabelle angezeigt
     */
    void receivedAllMails();
    /**
     * @brief Der POP3- oder der SMTP-Client haben sich vom Server getrennt
     */
    void quittedServer();
    /**
      * @brief Öffnet ein extra Fenster, zum Versenden einer Mail
      */
     void on_sendMailPushButton_clicked();
     /**
     * @brief Fordert den POP3-Client auf, Mails vom Server abzurufen
     */
    void on_receiveMailsPushButton_clicked();
    /**
     * @brief Der Benutzer wählt in der Tabelle eine Mail aus
     * @param row Die Tabellenreihe
     * @param column Die Tabllenspalte
     */
    void on_mailsTableWidget_cellClicked(int row, int column);
    /**
     * @brief Der Benutzer makiert die aktuell ausgewählte Mail zum Löschen
     */
    void on_deleteMailPushButton_clicked();
    /**
     * @brief Der Benutzer verwirft alle Änderungen (das Löschen der Mails)
     */
    void on_resetPushButton_clicked();

private:
    Ui::MenuWidget *ui;
    /**
     * @brief Der POP3-Client
     */
    Pop3Client *pop3Client;
    /**
     * @brief Der SMTP-Client
     */
    SmtpClient *smtpClient;
    /**
     * @brief Die Mail, die gerade in der Tabelle ausgewählt ist
     */
    int selectedMailIndex;

    /**
     * @brief Zeigt den Mailinhalt der aktuell ausgewähtlen Mail an
     * @param mailIndex
     */
    void showMail(int mailIndex);
    /**
     * @brief Überschreibt das Schließen des Fenster, damit dabei die Verbindungen zum SMTP- und POP3-Server geschlossen wird
     * @param event Das Event
     */
    void closeEvent(QCloseEvent *event);

};
#endif // MENUWIDGET_H

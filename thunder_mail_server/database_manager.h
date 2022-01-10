#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QDir>

/**
 * @brief In dieser Struktur werden die Mails an den POP3-Server übergeben.
 */
struct DatabaseMail
{
    /**
     * @brief Die ID der Mail
     */
    int mailId;
    /**
     * @brief Der Mailinhalt
     */
    QString mailContent;
};

/**
 * @brief Diese Klasse verwaltet die SQLite-Datenbank, in der die Benutzer und die versendeten Mails gespeichert werden
 */
class DatabaseManager
{
public:
    DatabaseManager();

    /**
     * @brief Öffnet die Datenbank-Sizung
     */
    void openDatabase();
    /**
     * @brief Überpüft, ob ein Benutzer in der Datenbank exestiert
     * @param username Der Benutzername
     * @return  Die ID des Benutzers, wenn er nicht exestiert -1
     */
    int userExists(QString username);
    /**
     * @brief Überprüft, ob die Anmeldedaten korrekt sind
     * @param username Die Id des Benutzers
     * @param password Das Passwort
     * @return true, wenn die Anmeldeaten korrekt sind, ansonsten false
     */
    bool loginDataIsCorrect(int userId, QString password);
    /**
     * @brief Sucht alle Mails, die an einen Benutzer geschickt wurden
     * @param userId Die ID des Benutzers
     * @return Die Mails
     */
    QList<DatabaseMail> getUsersMails(int userId);
    /**
     * @brief Sucht den Inhalt einer Mail
     * @param recipientId Die ID in der Tabelle "mail"
     * @return Der Mailinhalt
     */
    QString getMailContent(int mailId);
    /**
     * @brief Speichert eine Mail in der Datenbank und verlinkt sie zu allen Benutzern, den sie geschickt wurde
     * @param recipients Die Empänger
     * @param content Der Mailinhalt
     */
    void storeMail(QStringList recipients, QString content);
    /**
     * @brief Löscht die Verlinkung einer Mail zu dem Empänger. Ist er der einzige Empänger, wird die Mail gelöscht.
     * @param userId Die ID des Benutzers
     * @param mailId Die ID der Mail
     */
    void delteteMail(int userId, int mailId);
private:
    /**
     * @brief Pfad, wo die SQLite-Datenbank abgespeichert ist.
     */
    QString path;
    /**
     * @brief Die Datenbankinstanz
     */
    QSqlDatabase database;
    /**
     * @brief Ermöglicht Abfragen an die Datenbank
     */
    QSqlQuery query;
};

#endif // DATABASEMANAGER_H

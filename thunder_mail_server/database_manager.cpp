#include "database_manager.h"
#include <QSqlError>

DatabaseManager::DatabaseManager()
    : path(QDir::currentPath() + "/../thunder_mail_server/thunder_mail_server.db")
    , database(QSqlDatabase::addDatabase("QSQLITE"))
    , query(QSqlQuery(database))
{
    openDatabase();
}

void DatabaseManager::openDatabase()
{
    database.setDatabaseName(path);
    if(!database.open())
    {
        qWarning() << "Beim Öffnen der Datenbank tratt ein Fehler auf!";
    }
}

bool DatabaseManager::userExists(QString username)
{
    query.prepare("SELECT * FROM user WHERE username = ?;");
    query.bindValue(0, username);
    query.exec();
    return query.first();
}

int DatabaseManager::loginDataIsCorrect(int userId, QString password)
{
    query.prepare("SELECT user_id FROM user WHERE user_id = ? AND password = ?;");
    query.bindValue(0, userId);
    query.bindValue(1, password);
    query.exec();
    if(query.first())
        return query.value(0).toInt();
    return -1;
}

QList<DatabaseMail> DatabaseManager::getUsersMails(int userId)
{
    QList<DatabaseMail> mails;
    query.prepare("SELECT mail.mail_id, mail.content FROM mail INNER JOIN recipient ON mail.mail_id = recipient.mail_id WHERE recipient.user_id = ?;");
    query.bindValue(0, userId);
    query.exec();
    while (query.next())
    {
        DatabaseMail mail;
        mail.mailId = query.value(0).toInt();
        mail.mailContent = query.value(1).toString();
        mails.append(mail);
    }
    return mails;
}

QString DatabaseManager::getMailContent(int mailId)
{
    query.prepare("SELECT content FROM mail WHERE mail_id = ?;");
    query.bindValue(0, mailId);
    query.exec();
    if(query.first())
        return query.value(0).toString();
    return "";
}

void DatabaseManager::storeMail(QStringList recipients, QString content)
{
    //Fügt die Mail in der Datenbank ein
    query.prepare("INSERT INTO mail (content) VALUES (?);");
    query.bindValue(0, content);
    query.exec();
    //Ermittelt den neu erzeugten Index der Mail
    query.exec("SELECT MAX(mail_id) FROM mail;");
    if(query.first())
    {
        int newMailIndex = query.value(0).toInt();
        //Verknüpft aller Benutzer mit der Mail, die Empfänger sind
        for(int i = 0; i < recipients.size(); i++)
        {
            query.prepare("INSERT INTO recipient (user_id, mail_id) VALUES((SELECT user_id FROM user WHERE username = ?), ?);");
            query.bindValue(0, recipients.at(i));
            query.bindValue(1, newMailIndex);
            query.exec();
        }
    }
}

void DatabaseManager::delteteMail(int userId, int mailId)
{
    //Löscht die Verlinkung der Mail zu dem Benutzer
    query.prepare("DELETE FROM recipient WHERE user_id = ? AND mail_id = ?;");
    query.bindValue(0, userId);
    query.bindValue(1, mailId);
    query.exec();
    //Überpüft ob noch andere Benutzer diese Mail empfangen haben
    query.prepare("SELECT * FROM recipient WHERE mail_id = ?;");
    query.bindValue(0, mailId);
    query.exec();
    //Wenn nicht, wird die Mail auch aus der Tabelle "mail" gelöscht
    if(!query.first())
    {
        query.prepare("DELETE FROM mail WHERE mail_id = ?;");
        query.bindValue(0, mailId);
        query.exec();
    }
}


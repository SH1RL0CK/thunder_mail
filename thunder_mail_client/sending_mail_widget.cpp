#include "sending_mail_widget.h"
#include "ui_sending_mail_widget.h"

SendingMailWidget::SendingMailWidget(SmtpClient *_smtpClient, QString sender, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SendingMailWidget)
    , smtpClient(_smtpClient)
{
    ui->setupUi(this);
    // Der Absender der Mail ist immer der Benutzer, der sich angemeldet hat.
    ui->senderLineEdit->setText(sender);
    ui->senderLineEdit->setDisabled(true);
    QObject::connect(smtpClient, &SmtpClient::sendingMailWasSucessful, this, &SendingMailWidget::sendingMailWasSuccessful);
    QObject::connect(smtpClient, &SmtpClient::sendingMaiFailed, this, &SendingMailWidget::sendingMailFailed);
}

SendingMailWidget::~SendingMailWidget()
{
    delete ui;
}

void SendingMailWidget::on_sendMailPushButton_clicked()
{
    ui->feedbackOutput->setText("");
    QString sender = ui->senderLineEdit->text();
    QStringList recipients = ui->recipientsLineEdit->text().split(",");
    QString subject = ui->subjectLineEdit->text();
    QString body = ui->bodyPlainTextEdit->toPlainText();

    // Überprüft die Mail-Adressen.
    if(!mailsAreValid(sender, recipients))
        return;

    smtpClient->sendMail(sender, recipients, subject, body);
    // Der Benutzer kann nun nichts mehr eingeben.
    setInputsDisabled(true);
    showFeedbackOutput("Die Mail wird versendet...", "orange");
}

void SendingMailWidget::sendingMailWasSuccessful()
{
    showFeedbackOutput("Die Mail wurde erfolgreich versand!", "green");
    QMessageBox::information(this, "Erfolgreich!", "Die Mail wurde erfolgreich versendet!");
    this->close();
    smtpClient->disconnect();
}

void SendingMailWidget::sendingMailFailed()
{
    // Fordert den Server auf, die angefangene Übertragung der Mail abzubrechen.
    smtpClient->sendResetRequest();
    showFeedbackOutput("Beim Versenden der Mail trat ein Fehler auf!", "red");
    // Der Benutzer kann nun wieder Eingaben tätigen.
    setInputsDisabled(false);
    QMessageBox::warning(this, "Fehler!", "Beim Versenden der Mail trat ein Fehler auf!\nAntwort vom Server: " + smtpClient->getLastestError());
}

void SendingMailWidget::showFeedbackOutput(QString feedback, QString color)
{
    ui->feedbackOutput->setStyleSheet("color: " + color);
    ui->feedbackOutput->setText(feedback);
}

bool SendingMailWidget::mailsAreValid(QString sender, QStringList recipients)
{
    // Zum Überpüfen der Mailadresse wird ein regulärer Ausdruck verwendet.
    QRegExp mailRegex("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b");
    mailRegex.setCaseSensitivity(Qt::CaseInsensitive);
    mailRegex.setPatternSyntax(QRegExp::RegExp);

    if(!mailRegex.exactMatch(sender))
    {
        showFeedbackOutput("Gib als Absender bitte eine richtige Mail-Adresse an!", "red");
        return false;
    }
    for(int i = 0; i < recipients.size(); i++)
    {
        if(!mailRegex.exactMatch(recipients.at(i)))
        {
            showFeedbackOutput("Gib als Empänger bitte (eine) richtige Mail-Adresse(n) an!\n(Hinweis: Mehrere Empfänger mit \",\" trennen z.B. alice@thunder-mail.de,bob@thunder-mail.de,...)", "red");
            return false;
        }
    }
    return true;
}

void SendingMailWidget::setInputsDisabled(bool disabled)
{
    ui->recipientsLineEdit->setDisabled(disabled);
    ui->subjectLineEdit->setDisabled(disabled);
    ui->bodyPlainTextEdit->setDisabled(disabled);
    ui->sendMailPushButton->setDisabled(disabled);
}

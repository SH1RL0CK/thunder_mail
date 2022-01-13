#include "login_widget.h"
#include "ui_login_widget.h"

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget)
    , pop3Client(new Pop3Client)
    , smtpClient(new SmtpClient)
{
    ui->setupUi(this);
    QObject::connect(pop3Client, &Pop3Client::verifiedSuccessfully, this, &LoginWidget::clientWasVerifiedSuccessfully);
    QObject::connect(pop3Client, &Pop3Client::verificationFailed, this, &LoginWidget::loginFailed);
    QObject::connect(smtpClient, &SmtpClient::connectedToServerSuccessfully, this, &LoginWidget::clientWasVerifiedSuccessfully);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::on_loginPushButton_clicked()
{
    ui->pop3OutputLabel->setText("");
    ui->thunderMailServerAdressLineEdit->setDisabled(true);
    ui->smtpSeverPortSpinBox->setDisabled(true);
    ui->pop3ServerPortSpinBox->setDisabled(true);
    ui->pop3ServerUsernameLineEdit->setDisabled(true);
    ui->pop3ServerPasswordLineEdit->setDisabled(true);

    QString address = ui->thunderMailServerAdressLineEdit->text();
    int pop3Port = ui->pop3ServerPortSpinBox->value();
    QString pop3Username = ui->pop3ServerUsernameLineEdit->text();
    QString pop3Password = ui->pop3ServerPasswordLineEdit->text();
    int smtpPort = ui->smtpSeverPortSpinBox->value();
    pop3Client->connectToServer(address, pop3Port, pop3Username, pop3Password);
    smtpClient->connectToServer(address, smtpPort);
}

void LoginWidget::clientWasVerifiedSuccessfully()
{
    if(pop3Client->getState() == Pop3ClientState::Pop3VerifiedAtServer && smtpClient->getState() == SmtpClientState::SmtpVerifiedAtServer)
    {
        pop3Client->disconnect();
        smtpClient->disconnect();
        MenuWidget *menuWidget = new MenuWidget(pop3Client, smtpClient);
        menuWidget->show();
        this->hide();
        this->close();
    }
}

void LoginWidget::loginFailed()
{
    ui->pop3ServerUsernameLineEdit->setDisabled(false);
    ui->pop3ServerPasswordLineEdit->setDisabled(false);
    ui->pop3OutputLabel->setText("Der Benutzername oder das Passwort sind falsch!");
}

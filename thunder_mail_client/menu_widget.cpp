#include "menu_widget.h"
#include "ui_menu_widget.h"

MenuWidget::MenuWidget(Pop3Client *_pop3Client, SmtpClient *_smtpClient, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MenuWidget)
    , pop3Client(_pop3Client)
    , smtpClient(_smtpClient)
    , selectedMailIndex(-1)
{
    ui->setupUi(this);
    QObject::connect(pop3Client, &Pop3Client::receivedAllMails, this, &MenuWidget::receivedAllMails);
    pop3Client->getAllMails();
    // Die Spaltengröße der Tabelle mit den Mails, passt sich der Gesmatgröße der Tabelle an
    ui->mailsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->mailsTableWidget->setEditTriggers(QAbstractItemView::EditTriggers(0));
}

MenuWidget::~MenuWidget()
{
    delete ui;
}

void MenuWidget::on_sendMailPushButton_clicked()
{
    SendingMailWidget *sendingMailWidget = new SendingMailWidget(smtpClient, pop3Client->getUsername());
    sendingMailWidget->show();
}

void MenuWidget::receivedAllMails()
{
    QList<Pop3ClientMail> mails = pop3Client->getReceivedMails();
    ui->mailsTableWidget->setRowCount(0);
    ui->mailsTableWidget->setRowCount(mails.size());
    for (int i = 0; i < mails.size(); i++)
    {
        ui->mailsTableWidget->setItem(i, 0, new QTableWidgetItem(mails.at(i).sender));
        ui->mailsTableWidget->setItem(i, 1, new QTableWidgetItem(mails.at(i).subject));
        ui->mailsTableWidget->setItem(i, 2, new QTableWidgetItem(mails.at(i).dateTime));
        ui->mailsTableWidget->setItem(i, 3, new QTableWidgetItem(mails.at(i).marktForDelete ? "Ja" : "Nein"));
    }
    showMail(0);
}

void MenuWidget::on_receiveMailsPushButton_clicked()
{
    pop3Client->getAllMails();
}

void MenuWidget::on_mailsTableWidget_cellClicked(int row, int column)
{
    showMail(row);
}

void MenuWidget::showMail(int mailIndex)
{
    selectedMailIndex = mailIndex;
    Pop3ClientMail mail = pop3Client->getReceivedMails().at(mailIndex);
    ui->senderLineEdit->setText(mail.sender);
    ui->recipientsLineEdit->setText(mail.recipients);
    ui->dateTimeLineEdit->setText(mail.dateTime);
    ui->subjectLineEdit->setText(mail.subject);
    ui->bodyTextEdit->setPlainText(mail.body);
    ui->mailContentLabel->setText("Inhalt (Mail " + QString::number(mailIndex + 1) + "):");
}

void MenuWidget::closeEvent(QCloseEvent *bar)
{
    pop3Client->quit();
    smtpClient->sendQuitRequest();
    bar->accept();
}


void MenuWidget::on_deleteMailPushButton_clicked()
{
    //pop3Client->deleteMail(selectedMailIndex + 1);
    pop3Client->quit();
    smtpClient->sendQuitRequest();
}

void MenuWidget::on_resetPushButton_clicked()
{
    pop3Client->reset();
}

#include "menu_widget.h"
#include "ui_menu_widget.h"

MenuWidget::MenuWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MenuWidget)
{
    ui->setupUi(this);
    smptClient = new SmptClient(this);
    smptClient->connectToServer("localhost", 4711);
}

MenuWidget::~MenuWidget()
{
    delete ui;
}


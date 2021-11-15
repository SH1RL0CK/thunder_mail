#include "administration_widget.h"
#include "ui_administration_widget.h"

AdministrationWidget::AdministrationWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AdministrationWidget)
{
    ui->setupUi(this);
}

AdministrationWidget::~AdministrationWidget()
{
    delete ui;
}


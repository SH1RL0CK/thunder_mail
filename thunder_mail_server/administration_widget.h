#ifndef ADMINISTRATIONWIDGET_H
#define ADMINISTRATIONWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class AdministrationWidget; }
QT_END_NAMESPACE

class AdministrationWidget : public QWidget
{
    Q_OBJECT

public:
    AdministrationWidget(QWidget *parent = nullptr);
    ~AdministrationWidget();

private:
    Ui::AdministrationWidget *ui;
};
#endif // ADMINISTRATIONWIDGET_H

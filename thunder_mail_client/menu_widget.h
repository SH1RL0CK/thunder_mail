#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MenuWidget; }
QT_END_NAMESPACE

class MenuWidget : public QWidget
{
    Q_OBJECT

public:
    MenuWidget(QWidget *parent = nullptr);
    ~MenuWidget();

private:
    Ui::MenuWidget *ui;
};
#endif // MENUWIDGET_H

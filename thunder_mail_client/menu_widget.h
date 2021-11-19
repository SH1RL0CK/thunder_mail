#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include <QWidget>
#include "smpt_client.h"

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
    MenuWidget(QWidget *parent = nullptr);
    ~MenuWidget();

private:
    Ui::MenuWidget *ui;

    SmptClient *smptClient;
};
#endif // MENUWIDGET_H

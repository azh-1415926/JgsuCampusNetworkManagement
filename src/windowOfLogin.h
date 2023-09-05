#ifndef WINDOWOFLOGIN_H
#define WINDOWOFLOGIN_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class windowOfLogin; }
QT_END_NAMESPACE

class windowOfLogin : public QWidget
{
    Q_OBJECT

private:
    Ui::windowOfLogin* ui;

public:
    windowOfLogin(QWidget *parent = nullptr);
    ~windowOfLogin();
};

#endif // WINDOWOFLOGIN_H
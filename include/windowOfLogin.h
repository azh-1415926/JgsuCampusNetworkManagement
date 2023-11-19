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

public slots:
    void loadUserInfo(const QString& account,const QString& passwd);
    void processLoginFailed(const QString& info);
    void processLoginSuccess(const QString& info);

private slots:
    void initalLoginBox();
    void login();
    void reset();

signals:
    void sendUserInfo(const QString& account,const QString& passwd);

private:
    ;
};

#endif // WINDOWOFLOGIN_H
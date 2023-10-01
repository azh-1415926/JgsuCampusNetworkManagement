#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "windowOfLogin.h"
#include "myHttp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class CurrentHost
{
    NONE=-1,
    AUTHENTICATION=0,
    MANAGEMENT=1
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    windowOfLogin* m_LoginBox;
    myHttp* m_Http;
    QString m_Account;
    QString m_Password;
    QString m_Cookie;
    int m_FlagOfInital;
    int m_FlagOfCookie;
    CurrentHost m_CurrHost;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* e) override;

public slots:
    ;

signals:
    void sendUserInfo(const QString& account,const QString& passwd);
    void loginFailed(const QString& info);
    void loginSuccess(const QString& info);
    void switchToNextHost();

private:
    void initalWindow();
    void handleUserInfo(const QString& account,const QString& passwd);
    void goToManagement();
    QString getCookie(QString response);
    void processResponse(const QString& response);
    void processAuthentication(const QString& response);
    void processManagement(const QString& response);
};

#endif // MAINWINDOW_H
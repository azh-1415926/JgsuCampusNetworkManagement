#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "windowOfLogin.h"
#include "myHttp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    windowOfLogin* m_login;
    myHttp* m_Http;
    int m_flagOfInital;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* e) override;

public slots:
    ;

signals:
    void loginFailed(const QString& info);
    void loginSuccess(const QString& info);

private:
    void initalWindow();
};

#endif // MAINWINDOW_H
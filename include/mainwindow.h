#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "windowOfLogin.h"
#include "myHttp.h"

class settingFile;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow* ui;
    windowOfLogin* m_LoginBox;
    myHttp* m_Http;
    settingFile* m_Info;
    QList<QString> m_Hosts;
    QList<QList<QJsonObject>> m_Messages;
    int m_FlagOfInital;
    int m_FlagOfCookie;
    QString m_CurrHost;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* e) override;

public slots:
    ;

private slots:
    void saveAttribute(const QString& key,const QString& value);
    void sendHttpMessage(const QString& message);
    void processResponse(const QString& response);
    void processAuthentication(const QString& response);
    void processManagement(const QString& response);

signals:
    void sendUserInfo(const QString& account,const QString& passwd);
    void logged(bool status,const QString& info);

private:
    void initalWindow();
    void initalSetting();
    QString processParam(const QString& method,const QString& param);
};

void saveFile(const QString& path,const QString& data);

#endif // MAINWINDOW_H
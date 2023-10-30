#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>


#include "settingFile.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_LoginBox(new windowOfLogin)
    , m_Http(new myHttp)
    , m_FlagOfInital(0)
    , m_FlagOfCookie(0)
    , m_CurrHost(CurrentHost::NONE)
{
    ui->setupUi(this);
    initalWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_LoginBox;
    delete m_Http;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    if(m_FlagOfInital==0)
    {
        settingFile setting;
        setting.load("passwd.json");
        if(setting.isLoad())
            sendUserInfo(setting.value("user_account").toString(),setting.value("user_password").toString());
        m_LoginBox->show();
        m_FlagOfInital=1;
        m_CurrHost=CurrentHost::AUTHENTICATION;
    }
}

void MainWindow::initalWindow()
{
    connect(this,&MainWindow::sendUserInfo,m_LoginBox,&windowOfLogin::loadUserInfo);
    connect(m_LoginBox,&windowOfLogin::sendUserInfo,this,handleUserInfo);
    connect(m_Http,&myHttp::failed,this,[=](const QString& error)
    {
        qDebug()<<error;
    });
    connect(m_Http,&myHttp::recv,this,processResponse);
    connect(this,&MainWindow::loginFailed,m_LoginBox,&windowOfLogin::processLoginFailed);
    connect(this,&MainWindow::loginSuccess,m_LoginBox,&windowOfLogin::processLoginSuccess);
    connect(this,&MainWindow::loginSuccess,this,goToManagement);
}

void MainWindow::handleUserInfo(const QString &account, const QString &passwd)
{
    m_Account=account;
    m_Password=passwd;
    QString url="/eportal/portal/login?user_account=%2C0%2C"
                + account + "&user_password=" + passwd;
    QList<QPair<QString,QString>> fields;
    fields.push_back(QPair<QString,QString>("Accept","text/html"));
    m_Http->send("get",url,QPair<QString,int>("192.168.77.18",801),fields);
}

void MainWindow::goToManagement()
{
    QString url="/Self/login/?302=LI";
    QList<QPair<QString,QString>> fields;
    // fields.push_back(QPair<QString,QString>("Accept","text/html"));
    fields.push_back(QPair<QString,QString>("User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWe"));
    m_Http->send("get",url,QPair<QString,int>("192.168.168.15",8080),fields);
}

QString MainWindow::getCookie(QString response)
{
    QString line;
    QTextStream text(&response);
    while(!(line =text.readLine()).isNull())
        if(line.contains("Set-Cookie"))
            return line;
    return QString();
}

void MainWindow::getOnlineList()
{
    // QString url="/Self/dashboard/getOnlineList?t=0.11768364794494879&order=asc&_=1695653835657";
    QString url="/Self/dashboard/getOnlineList?order=asc";
    QList<QPair<QString,QString>> fields;
    fields.push_back(QPair<QString,QString>("Cookie",m_Cookie));
    m_Http->send("get",url,QPair<QString,int>("192.168.168.15",8080),fields);
}

void MainWindow::logout()
{
    QString url="/Self/login/logout";
    QList<QPair<QString,QString>> fields;
    fields.push_back(QPair<QString,QString>("Cookie",m_Cookie));
    m_Http->send("get",url,QPair<QString,int>("192.168.168.15",8080),fields);
}

void MainWindow::processResponse(const QString &response)
{
    qDebug()<<"Response : "<<response;
    switch (m_CurrHost)
    {
    case CurrentHost::AUTHENTICATION:
        if(response.isEmpty())
            emit loginFailed("loginFailed!");
        else
            processAuthentication(response);
        break;

    case CurrentHost::MANAGEMENT:
        processManagement(response);
        break;

    default:
        break;
    }
}

void MainWindow::processAuthentication(const QString& response)
{
    int begin=response.indexOf("{");
    int end=response.lastIndexOf("}");
    if(begin!=-1&&end!=-1)
    {
        QString str=response.mid(begin,end+1-begin);
        qDebug()<<"json : "<<str;
        QJsonParseError error;
        QJsonDocument doc=QJsonDocument::fromJson(str.toUtf8(),&error);
        if(error.error!=QJsonParseError::NoError&&!doc.isNull())
        {
            qDebug()<<"json parse error","json 格式错误!";
            return;
        }
        QJsonObject json=doc.object();
        int ret=json.value("ret_code").toInt();
        QString msg=json.value("msg").toString();
        qDebug()<<"ret_code : "<<ret;
        if(ret!=2&&ret!=0)
            emit loginFailed(msg);
        else
        {
            m_CurrHost=CurrentHost::MANAGEMENT;
            settingFile setting;
            setting.add("user_account",m_Account);
            setting.add("user_password",m_Password);
            setting.save("passwd.json");
            emit loginSuccess(msg);
        }
    }
}

void MainWindow::processManagement(const QString& response)
{
    if(m_FlagOfCookie==0)
    {
        m_Cookie=getCookie(QString(response));
        m_FlagOfCookie=1;
        int begin=m_Cookie.indexOf(":");
        int end=m_Cookie.indexOf(";");
        m_Cookie=m_Cookie.mid(begin+1,end-1-begin).trimmed();
        qDebug()<<"Cookie : "<<m_Cookie;
        QString encryptPasswd= QCryptographicHash::hash(m_Password.toLatin1(),QCryptographicHash::Md5).toHex();
        QString url="/Self/login/verify";
        QList<QPair<QString,QString>> fields;
        // QString content="foo=&bar=&account="+m_Account+"&password="+encryptPasswd+"&code=";
        QString content="foo=&bar=&account="+m_Account+"&password="+m_Password+"&code=";
        fields.push_back(QPair<QString,QString>("Cookie",m_Cookie));
        m_Http->send("post",url,QPair<QString,int>("192.168.168.15",8080),fields,content);
        getOnlineList();
    }
    else
        m_CurrHost=CurrentHost::NONE;
}
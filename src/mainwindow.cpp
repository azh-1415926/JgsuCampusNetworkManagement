#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QJsonObject>
#include <QJsonDocument>

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
        m_LoginBox->show();
        m_FlagOfInital=1;
    }
}

void MainWindow::initalWindow()
{
    m_CurrHost=CurrentHost::AUTHENTICATION;
    connect(m_LoginBox,&windowOfLogin::sendInfo,this,handleUserInfo);
    connect(m_Http,&myHttp::failed,this,[=](const QString& error)
    {
        qDebug()<<error;
    });
    connect(m_Http,&myHttp::recv,this,processResponse);
    connect(m_Http,&myHttp::readed,this,switchHost);
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
    m_Http->send("get",url,QPair<QString,int>("192.168.167.42",801),fields);
}

void MainWindow::goToManagement()
{
    // should save passwd
    QString url="/Self/login/?302=LI";
    QList<QPair<QString,QString>> fields;
    // fields.push_back(QPair<QString,QString>("Accept","*/*"));
    // fields.push_back(QPair<QString,QString>("User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.0.0 Safari/537.36 Edg/117.0.2045.36"));
    m_Http->send("get",url,QPair<QString,int>("192.168.168.15",8080),fields);
}

void MainWindow::switchHost()
{
    if(m_CurrHost==CurrentHost::AUTHENTICATION)
        m_CurrHost=CurrentHost::MANAGEMENT;
    else if(m_FlagOfCookie&&m_CurrHost==CurrentHost::AUTHENTICATION)
        m_CurrHost=CurrentHost::NONE;
    else
        m_FlagOfCookie=1;
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

void MainWindow::processResponse(const QString &response)
{
    qDebug()<<"Response : "<<response;
    switch (m_CurrHost)
    {
    case CurrentHost::AUTHENTICATION:
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
        if(ret!=2)
            emit loginFailed(msg);
        else
        {
            m_CurrHost=CurrentHost::MANAGEMENT;
            emit loginSuccess(msg);
        }
    }
}

void MainWindow::processManagement(const QString& response)
{
    if(m_FlagOfCookie==0)
    {
        m_Cookie=getCookie(QString(response));
        int begin=m_Cookie.indexOf(":");
        int end=m_Cookie.indexOf(";");
        m_Cookie=m_Cookie.mid(begin+1,end-1-begin).trimmed();
        qDebug()<<"Cookie : "<<m_Cookie;
    }
}
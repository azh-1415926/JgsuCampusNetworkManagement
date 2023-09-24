#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_login(new windowOfLogin)
    , m_Http(new myHttp)
    , m_flagOfInital(0)
{
    ui->setupUi(this);
    initalWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_login;
    delete m_Http;
    delete m_login;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    if(m_flagOfInital==0)
    {
        m_login->show();
        m_flagOfInital=1;
    }
}

void MainWindow::initalWindow()
{
    connect(m_login,&windowOfLogin::sendInfo,this,[=](const QString& account,const QString& passwd)
    {
        QString url="/eportal/portal/login?user_account=%2C0%2C"
                    + account + "&user_password=" + passwd;
        QList<QPair<QString,QString>> fields;
        qDebug()<<"url : "+url;
        qDebug()<<"field : \n"<<fields;
        m_Http->send("get",url,QPair<QString,int>("192.168.167.42",801),fields);
    });
    connect(m_Http,&myHttp::failed,[=](const QString& error)
    {
        qDebug()<<error;
    });
    connect(m_Http,&myHttp::recv,[=](const QString& response)
    {
        qDebug()<<"Response : "<<response;
    });
}
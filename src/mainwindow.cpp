#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QFile>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

#include "settingReader.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_LoginBox(new windowOfLogin)
    , m_Http(new myHttp)
    , m_Info(new settingFile)
    , m_FlagOfInital(0)
    , m_FlagOfCookie(0)
{
    ui->setupUi(this);
    /* 初始化设置，导入需要发送的 http 请求信息 */
    initalSetting();
    /* 初始化窗口，在第一次显示窗口的时候弹出登录框 */
    initalWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_LoginBox;
    delete m_Http;
    delete m_Info;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    /* 只执行一次 */
    if(m_FlagOfInital==0)
    {
        /* 发送账号密码信息到 m_LoginBox，并显示，即登录框 */
        sendUserInfo(m_Info->value("account").toString(),m_Info->value("password").toString());
        m_LoginBox->show();
        m_FlagOfInital=1;
    }
}

/* 保存属性的值，保存到 info.json 文件 */
void MainWindow::saveAttribute(const QString &key, const QString &value)
{
    m_Info->add(key,value);
    m_Info->save("info.json");
}

/* 给定某个主机的关键字，发送其中的 http 请求 */
void MainWindow::sendHttpMessage(const QString& host)
{
    /* pos 为该关键字在所有主机中的位置，若为 -1，则不存在 */
    int pos=m_Hosts.indexOf(host);
    if(pos==-1)
        return;
    /* 取出该主机中所有的 http 报文对象 */
    for(int i=0;i<m_Messages.at(pos).length(); i++)
    {
        /* message 为该主机第 i 个报文对象 */
        auto message=m_Messages.at(pos).at(i);
        /* params 为该报文对象的参数对象 */
        auto params=message.value("params").toObject();
        /* 更新 params 中所有变量的值，从 m_Info 中取对应的值 */
        for(const auto& j : params.keys())
        {
            /* value 为关键字 j 的最终的值 */
            QString value;
            /* args 为分割后所有的子变量，如该关键字的值为 arg1+arg2，则拆成 arg1、arg2 */
            const auto& args=params.value(j).toString().split("+");
            for(const auto& k : args)
            {
                /* k 为子变量，可能需要被特殊处理（如 md5 加密） */
                const auto& strs=k.split("|");
                /* strs 为所有处理操作的集合（最后一个为需要处理的值），按照从右到左的顺序处理 */
                auto str=strs.at(strs.length()-1);
                for(int index=strs.length()-2;index>=0;index--)
                    str=processParam(strs.at(index),m_Info->value(str).toString());
                /* 拼接所有处理好的 arg（如果有需要的话） */
                value.append(str);
            }
            params.insert(j,value);
        }
        message.insert("params",params);
        const QString& hostInfo=message.value("fields").toObject().value("Host").toString();
        const QStringList& list=hostInfo.split(":");
        qDebug()<<"Message:"<<httpReader::load(message);
        m_Http->send(httpReader::load(message),QPair<QString,int>(list.at(0),list.at(1).toInt()));
    }
}

void MainWindow::processResponse(const QString &response)
{
    qDebug()<<"Response : "<<response;
    saveFile("log.txt",response);
    int pos=m_Hosts.indexOf(m_CurrHost);
    switch (pos)
    {
    case 0:
        if(response.isEmpty())
            emit logged(false,"loginFailed!");
        else
            processAuthentication(response);
        break;

    case 1:
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
        QJsonObject json=settingFile::toJson(response.mid(begin,end+1-begin));
        int ret=json.value("ret_code").toInt();
        QString msg=json.value("msg").toString();
        bool status=(ret!=2&&ret!=0)?false:true;
        logged(status,msg);
    }
}

void MainWindow::processManagement(const QString& response)
{
    if(m_FlagOfCookie==0)
    {
        QString str=response;
        QString checkcode;
        QRegularExpression re("<input\\s*type=\"hidden\" name=\"checkcode\" value=\"\\d{4}\">");
        QRegularExpressionMatch match=re.match(str);
        if(match.hasMatch())
        {
            checkcode=match.captured(0);
            qDebug()<<"matched:"<<checkcode;
            #if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
                checkcode=checkcode.mid(checkcode.length()-1-6,4);
            #elif (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
                checkcode=checkcode.last(6).first(4);
            #endif
        }
        else
            qDebug()<<"match failed!";
        qDebug()<<"checkcode:"<<checkcode;
        const QString& cookie=myHttp::getCookie(str);
        saveAttribute("checkcode",checkcode);
        saveAttribute("Cookie",cookie);
        m_FlagOfCookie=1;
    }
}

void MainWindow::initalSetting()
{
    QFile file1("host.json");
    settingFile host;
    if(!(file1.exists()))
    {
        host.load(":/json/host.json");
        host.save("host.json");
    }
    else
        host.load("host.json");
    const auto& list=settingReader::load(host.toJson());
    for(const auto& i : list)
    {
        m_Hosts.push_back(i.first);
        m_Messages.push_back(i.second);
    }
    QFile file2("info.json");
    if(!(file2.exists()))
        m_Info->load(":/json/info.json");
    else
        m_Info->load("info.json");
}

void MainWindow::initalWindow()
{
    connect(this,&MainWindow::sendUserInfo,m_LoginBox,&windowOfLogin::loadUserInfo);
    connect(m_LoginBox,&windowOfLogin::sendUserInfo,this,[=](const QString &account, const QString &password)
    {
        saveAttribute("account",account);
        saveAttribute("password",password);
        m_CurrHost=m_Hosts.at(0);
        sendHttpMessage(m_CurrHost);
    });
    connect(m_Http,&myHttp::failed,this,[=](const QString& error)
    {
        qDebug()<<error;
    });
    connect(m_Http,&myHttp::recv,this,&MainWindow::processResponse);
    connect(this,&MainWindow::logged,this,[=](bool status,const QString& info)
    {
        if(status)
        {
            m_LoginBox->processLoginSuccess(info);
            m_CurrHost=m_Hosts.at(1);
            sendHttpMessage(m_CurrHost);
        }
        else
            m_LoginBox->processLoginFailed(info);
    });
}

QString MainWindow::processParam(const QString &method, const QString &param)
{
    if(method=="Md5")
        return QCryptographicHash::hash(param.toLatin1(),QCryptographicHash::Md5).toHex();
    return param;
}

void saveFile(const QString &path,const QString& data)
{
    QFile file(path);
    /* 保存到 path 文件中（只写、追加保存） */
    if(file.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        /* stream 处理文件，设置编码为 utf-8 */
        QTextStream stream(&file);
        #if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
            stream.setCodec("utf-8");
        #elif (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
            stream.setEncoding(QStringConverter::Utf8);
        #endif
        /* 导出并关闭文件 */
        stream<<data;
        file.close();
    }
    else
        qDebug()<<"error","save \""+path+"\" is failed!";
}
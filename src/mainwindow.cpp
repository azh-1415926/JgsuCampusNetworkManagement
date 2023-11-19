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
    initalSetting();
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
    if(m_FlagOfInital==0)
    {
        sendUserInfo(m_Info->value("account").toString(),m_Info->value("password").toString());
        m_LoginBox->show();
        m_FlagOfInital=1;
    }
}

void MainWindow::saveAttribute(const QString &key, const QString &value)
{
    m_Info->add(key,value);
    m_Info->save("info.json");
}

void MainWindow::sendHttpMessage(const QString& host)
{
    int pos=m_Hosts.indexOf(host);
    if(pos==-1)
        return;
    for(int i=0;i<m_Messages.at(pos).length(); i++)
    {
        QJsonObject message=m_Messages.at(pos).at(i);
        QJsonObject params=message.value("params").toObject();
        for(const auto& j : params.keys())
        {
            QString value;
            const QStringList& strs=params.value(j).toString().split("+");
            for(const auto& k : strs)
                value.append(m_Info->value(k).toString());
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
        //QString encryptPasswd=QCryptographicHash::hash(m_Info->value("password").toString().toLatin1(),QCryptographicHash::Md5).toHex();
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
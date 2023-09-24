#include "myHttp.h"

static const QString errorPrefix="HTTP ERROR : ";
static const char* HTTP_VERSION="HTTP/1.1";

myHttp::myHttp(QObject *parent)
    : QObject(parent)
    , m_Socket(new QTcpSocket)
{
    initalHttp();
}

myHttp::~myHttp()
{
    delete m_Socket;
}

void myHttp::send(const QString& method,const QString& url,const QPair<QString,int> serverInfo,const QList<QPair<QString,QString>>& fields)
{
    QString str=method.toUpper();
    if(!isInvaildForMethod(str))
    {
        emit failed(errorPrefix+"method is invaild! Error method is "+str);
        return;
    }
    QString requestMessage;
    requestMessage.append(str+" "+url+" "+HTTP_VERSION+"\r\n");
    requestMessage.append("Host:"+serverInfo.first+":"+QString::number(serverInfo.second)+"\r\n");
    for(int i=0;i<fields.length();i++)
        requestMessage.append(fields[i].first+":"+fields[i].second+"\r\n");
    requestMessage.append("\n");
    emit failed("Request : "+ requestMessage);
    m_Socket->abort();
    m_Socket->connectToHost(serverInfo.first,serverInfo.second);
    if(!m_Socket->waitForConnected(2000))
        emit failed(errorPrefix+"Send time out!");
    m_Socket->write(requestMessage.toUtf8().constData(),requestMessage.length());
    if(!m_Socket->waitForReadyRead(2000))
        emit failed(errorPrefix+"Recv time out!");
    QByteArray data=m_Socket->readAll();
    QString response(data);
    emit recv(response);
    emit readed();
    m_Socket->close();
}

void myHttp::initalHttp()
{
    ;
}

bool myHttp::isInvaildForMethod(const QString &method)
{
    static QList<QString> methods=
    {
        "GET",      // 请求指定页面信息，并返回实体主体。
        "HEAD",     // 类似于get请求，只不过返回的响应中没有具体的内容，用于获取报头。
        "POST",     // 向指定资源提交数据进行处理请求（例如提交表单或上传文件），数据包含在请求体中。post请求可能会导致新的资源的建立或已有资源的修改。
        "PUT",      // 从客户端向服务器传送的数据取代指定的文档的内容。
        "DELETE",   // 请求服务器删除指定的页面。
        "CONNECT",  // HTTP/1.1协议中预留给能够将连接改为管道方式的代理服务器。
        "OPTIONS",  // 允许客户端查看服务器的性能。
        "TRACE"     // 回显服务器收到的请求，主要用于测试或诊断。
    };
    return methods.contains(method);
}

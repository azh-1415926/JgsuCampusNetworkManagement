#ifndef MYHTTP_H
#define MYHTTP_H

#include <QObject>
#include <QTcpSocket>

class myHttp : public QObject
{
    Q_OBJECT

private:
    QTcpSocket* m_Socket;

public:
    myHttp(QObject *parent = nullptr);
    ~myHttp();

public slots:
    void send(const QString& method,const QString& url,const QPair<QString,int> serverInfo,const QList<QPair<QString,QString>>& fields);

signals:
    void failed(const QString& error);
    void recv(const QString& response);

private:
    void initalHttp();
    bool isInvaildForMethod(const QString& method);
};

#endif // MYHTTP_H
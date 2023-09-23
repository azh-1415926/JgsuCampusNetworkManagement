#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <QObject>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class mySocket; }
QT_END_NAMESPACE

class mySocket : public QObject
{
    Q_OBJECT

private:
    Ui::mySocket* ui;
    QTcpSocket* m_Socket;

public:
    mySocket(QObject *parent = nullptr);
    ~mySocket();

public slots:
    void handleInfo(const QString& account,const QString& passwd);

signals:
    ;

private:
    void initalSocket();
};

#endif // MYSOCKET_H
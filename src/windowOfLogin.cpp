#include "windowOfLogin.h"
#include "./ui_windowOfLogin.h"

#include <QRegularExpressionValidator>

windowOfLogin::windowOfLogin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::windowOfLogin)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    initalLoginBox();
}

windowOfLogin::~windowOfLogin()
{
    delete ui;
}

void windowOfLogin::initalLoginBox()
{
    QRegularExpressionValidator* reg = new QRegularExpressionValidator(this);
    reg->setRegularExpression(QRegularExpression(QString("\\d{1,10}")));
    ui->inputOfAccount->setValidator(reg);
    connect(ui->buttonOfLogin,&QPushButton::clicked,this,[=]()
    {
        login();
    });
    connect(ui->buttonOfReset,&QPushButton::clicked,this,[=]()
    {
        reset();
    });
}

void windowOfLogin::login()
{
    static QString account=ui->inputOfAccount->text();
    static QString password=ui->inputOfPassword->text();
    qDebug()<<"account:"<<account;
    qDebug()<<"password:"<<password;
    emit sendInfo(account,password);
}

void windowOfLogin::reset()
{
    ui->inputOfAccount->clear();
    ui->inputOfPassword->clear();
}
#include "windowOfLogin.h"
#include "./ui_windowOfLogin.h"

#include <QRegularExpressionValidator>
#include <QMessageBox>

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

void windowOfLogin::loadUserInfo(const QString& account,const QString& passwd)
{
    ui->inputOfAccount->setText(account);
    ui->inputOfPassword->setText(passwd);
}

void windowOfLogin::processLoginFailed(const QString& info)
{
    QMessageBox::warning(this,"Login Failed!",info);
    reset();
}

void windowOfLogin::processLoginSuccess(const QString& info)
{
    QMessageBox::about(this,"Login Success!",info);
    close();
    QString account=ui->inputOfAccount->text();
    QString password=ui->inputOfPassword->text();
}

void windowOfLogin::initalLoginBox()
{
    QRegularExpressionValidator* reg = new QRegularExpressionValidator(this);
    reg->setRegularExpression(QRegularExpression(QString("\\d{1,10}")));
    ui->inputOfAccount->setValidator(reg);
    connect(ui->buttonOfLogin,&QPushButton::clicked,this,login);
    connect(ui->buttonOfReset,&QPushButton::clicked,this,reset);
}

void windowOfLogin::login()
{
    QString account=ui->inputOfAccount->text();
    QString password=ui->inputOfPassword->text();
    ui->inputOfAccount->setReadOnly(true);
    ui->inputOfPassword->setReadOnly(true);
    emit sendUserInfo(account,password);
}

void windowOfLogin::reset()
{
    ui->inputOfAccount->setReadOnly(false);
    ui->inputOfPassword->setReadOnly(false);
    ui->inputOfAccount->clear();
    ui->inputOfPassword->clear();
}
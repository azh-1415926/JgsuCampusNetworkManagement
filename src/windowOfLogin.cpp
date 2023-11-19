#include "windowOfLogin.h"
#include "./ui_windowOfLogin.h"

#include <QRegularExpressionValidator>
#include <QMessageBox>

windowOfLogin::windowOfLogin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::windowOfLogin)
{
    ui->setupUi(this);
    initalLoginBox();
}

windowOfLogin::~windowOfLogin()
{
    delete ui;
}

/* 载入账号、密码 */
void windowOfLogin::loadUserInfo(const QString& account,const QString& passwd)
{
    ui->inputOfAccount->setText(account);
    ui->inputOfPassword->setText(passwd);
}

/* 处理登录失败 */
void windowOfLogin::processLoginFailed(const QString& info)
{
    QMessageBox::warning(this,"Login Failed!",info);
    reset();
}

/* 处理登录成功 */
void windowOfLogin::processLoginSuccess(const QString& info)
{
    QMessageBox::about(this,"Login Success!",info);
    close();
}

/* 初始化登录框 */
void windowOfLogin::initalLoginBox()
{
    /* 设置登录框为模态 */
    setWindowFlags(windowFlags() | Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    /* 设置账号输入框只能为数字，且最长只能为 10 位 */
    QRegularExpressionValidator* reg = new QRegularExpressionValidator(this);
    reg->setRegularExpression(QRegularExpression(QString("\\d{1,10}")));
    ui->inputOfAccount->setValidator(reg);
    /* 连接登录按钮、重置按钮到槽函数 */
    connect(ui->buttonOfLogin,&QPushButton::clicked,this,&windowOfLogin::login);
    connect(ui->buttonOfReset,&QPushButton::clicked,this,&windowOfLogin::reset);
}

/* 登录操作 */
void windowOfLogin::login()
{
    QString account=ui->inputOfAccount->text();
    QString password=ui->inputOfPassword->text();
    ui->inputOfAccount->setReadOnly(true);
    ui->inputOfPassword->setReadOnly(true);
    emit sendUserInfo(account,password);
}

/* 重置操作 */
void windowOfLogin::reset()
{
    ui->inputOfAccount->setReadOnly(false);
    ui->inputOfPassword->setReadOnly(false);
    ui->inputOfAccount->clear();
    ui->inputOfPassword->clear();
}
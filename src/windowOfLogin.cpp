#include "windowOfLogin.h"
#include "./ui_windowOfLogin.h"

windowOfLogin::windowOfLogin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::windowOfLogin)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
}

windowOfLogin::~windowOfLogin()
{
    delete ui;
}
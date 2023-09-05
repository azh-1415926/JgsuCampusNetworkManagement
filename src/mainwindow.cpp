#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_login(new windowOfLogin)
    , m_flagOfInital(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_login;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    if(m_flagOfInital==0)
        m_login->show();
}
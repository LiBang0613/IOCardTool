#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include "logexport.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ManageLog::getLogExport();
    m_daqSetPage = new DaqSet(this);
    m_multipltSetPage = new MultipleSet(this);
    ui->tabWidget->addTab(m_daqSetPage,QStringLiteral("数采卡测试"));
    ui->tabWidget->addTab(m_multipltSetPage,QStringLiteral("设备测试"));
    qDebug()<<"main"<<QThread::currentThreadId();
}

MainWindow::~MainWindow()
{
    ManageLog::deleteManageLog();
    delete ui;
}

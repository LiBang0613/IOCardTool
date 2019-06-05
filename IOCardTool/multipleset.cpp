#include "multipleset.h"
#include "ui_multipleset.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

MultipleSet::MultipleSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultipleSet)
{
    ui->setupUi(this);
    ui->le_ip1->setValidator(new QIntValidator(0,255,ui->le_ip1));
    ui->le_ip2->setValidator(new QIntValidator(0,255,ui->le_ip2));
    ui->le_ip3->setValidator(new QIntValidator(0,255,ui->le_ip3));
    ui->le_ip4->setValidator(new QIntValidator(0,255,ui->le_ip4));
}

MultipleSet::~MultipleSet()
{
    delete ui;
}

void MultipleSet::on_pb_saveInfo_clicked()
{
    if(!judgeSettingInfo())
    {
        return;
    }
    int row = ui->table_Info->rowCount();
    ui->table_Info->insertRow(row);
    int num1211,num1240;
    switch (ui->comB_deviceType->currentIndex())
    {
    case dtDevice1:
            num1211 = 2;
            num1240 = 1;
        break;
    case dtDevice2:
            num1211 = 3;
            num1240 = 0;
        break;
    case dtDevice3:
            num1211 = 0;
            num1240 = 3;
        break;
    case dtDevice4:
            num1211 = 1;
            num1240 = 2;
        break;
    }
    ui->table_Info->setItem(row,0,new QTableWidgetItem(ui->comB_deviceType->currentText().trimmed()));
    ui->table_Info->setItem(row,1,new QTableWidgetItem(getIpAddr()));
    ui->table_Info->setItem(row,2,new QTableWidgetItem(QString::number(num1211)));
    ui->table_Info->setItem(row,3,new QTableWidgetItem(QString::number(num1240)));
}

void MultipleSet::on_pb_start_clicked()
{
    if(ui->table_Info->currentRow() != -1)
    {
        QString ip = ui->table_Info->item(ui->table_Info->currentRow(),1)->text();
        if(m_mapCardRunTime.contains(ip) && m_mapCardRunTime.value(ip).bJudge == true)
        {
            QMessageBox::information(this,"提示","请勿重复点击开始测试。","确定");
            return;
        }
        RunTime time;
        time.bJudge = true;
        time.strBeginTime = QDateTime::currentDateTime().toString("yyyyMMDDhhmmss");
        m_mapCardRunTime.insert(ip,time);
    }
    else
    {
        QMessageBox::information(this,"提示","点击开始前请先选中表格中某行数据。","确定");
    }
}

void MultipleSet::on_pb_stop_clicked()
{
    if(ui->table_Info->currentRow() != -1)
    {
        QString ip = ui->table_Info->item(ui->table_Info->currentRow(),1)->text();
        if(m_mapCardRunTime.contains(ip) && m_mapCardRunTime[ip].bJudge == true)
        {
            m_mapCardRunTime[ip].bJudge = false;
        }
    }
    else
    {
        QMessageBox::information(this,"提示","点击停止前请先选中表格中某行数据。","确定");
    }
}

void MultipleSet::slt_clearTextEdit()
{
    ui->te_showMsg->clear();
}

bool MultipleSet::judgeSettingInfo()
{
    if(ui->le_ip1->text().trimmed().isEmpty() ||
            ui->le_ip2->text().trimmed().isEmpty() ||
            ui->le_ip3->text().trimmed().isEmpty() ||
            ui->le_ip4->text().trimmed().isEmpty())
    {
        QMessageBox::information(this,"提示","请输入正确Ip地址。","确定");
        return false;
    }

    for(int i=0;i<ui->table_Info->rowCount();++i)
    {
        if(ui->table_Info->item(i,1)->text() == getIpAddr())
        {
            QMessageBox::information(this,"提示","该ip地址已存在，请重新输入。","确定");
            return false;
        }
    }

    return true;
}

QString MultipleSet::getIpAddr()
{
    QString ip = ui->le_ip1->text().trimmed()+"."+ui->le_ip2->text().trimmed()+"."
            +ui->le_ip3->text().trimmed()+"."+ui->le_ip4->text().trimmed();
    return ip;
}


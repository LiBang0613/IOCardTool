﻿#include "daqset.h"
#include "ui_daqset.h"
#include <QDebug>
#include <QButtonGroup>
#define MOXA "MOXA"
#define SMACQ "Smacq"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


DaqSet::DaqSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DaqSet)
{
    ui->setupUi(this);
    QButtonGroup *group1 = new QButtonGroup(this);
    group1->addButton(ui->rb_Moxa);
    group1->addButton(ui->rb_smacq);

    QButtonGroup *group2 = new QButtonGroup(this);
    group2->addButton(ui->rb_485);
    group2->addButton(ui->rb_net);

    connect(&m_clearTextTimer,SIGNAL(timeout()),this,SLOT(slt_clearTxetEdit()));
    m_clearTextTimer.start(1000*60*2);

    ui->le_ip1->setValidator(new QIntValidator(0,255,ui->le_ip1));
    ui->le_ip2->setValidator(new QIntValidator(0,255,ui->le_ip2));
    ui->le_ip3->setValidator(new QIntValidator(0,255,ui->le_ip3));
    ui->le_ip4->setValidator(new QIntValidator(0,255,ui->le_ip4));
}

DaqSet::~DaqSet()
{
    delete ui;
}

void DaqSet::on_pb_saveInfo_clicked()
{
    if(!judgeSettingInfo())
        return;

    int row = ui->table_Msg->rowCount();
    ui->table_Msg->insertRow(row);
    QString strName;
    if(ui->rb_Moxa->isChecked())
    {
        strName = MOXA;
    }
    else if(ui->rb_smacq->isChecked())
    {
        strName = SMACQ;
    }
    ui->table_Msg->setItem(row,0,new QTableWidgetItem(strName));
    ui->table_Msg->setItem(row,1,new QTableWidgetItem(ui->comB_cardType->currentText().trimmed()));
    QString ip = getIpAddr();
    ui->table_Msg->setItem(row,2,new QTableWidgetItem(ip));
    ui->table_Msg->setItem(row,3,new QTableWidgetItem(ui->le_timeInterval->text().trimmed()));
    ui->table_Msg->horizontalHeader()->show();
}

void DaqSet::on_pb_start_clicked()
{
    qDebug()<<ui->table_Msg->currentRow();
    if(ui->table_Msg->currentRow() != -1)
    {
        qDebug()<<ui->table_Msg->item(ui->table_Msg->currentRow(),0)->text()
               <<ui->table_Msg->item(ui->table_Msg->currentRow(),1)->text()
              <<ui->table_Msg->item(ui->table_Msg->currentRow(),2)->text()
             <<ui->table_Msg->item(ui->table_Msg->currentRow(),3)->text();

        QString ip = ui->table_Msg->item(ui->table_Msg->currentRow(),2)->text();
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

void DaqSet::on_pb_stop_clicked()
{
    if(ui->table_Msg->currentRow() != -1)
    {
        qDebug()<<ui->table_Msg->item(ui->table_Msg->currentRow(),0)->text()
               <<ui->table_Msg->item(ui->table_Msg->currentRow(),1)->text()
              <<ui->table_Msg->item(ui->table_Msg->currentRow(),2)->text()
             <<ui->table_Msg->item(ui->table_Msg->currentRow(),3)->text();
        QString ip = ui->table_Msg->item(ui->table_Msg->currentRow(),2)->text();
        if(m_mapCardRunTime.contains(ip))
        {
            m_mapCardRunTime[ip].bJudge = false;
        }
    }
    else
    {
        QMessageBox::information(this,"提示","点击停止前请先选中表格中某行数据。","确定");
    }
}

void DaqSet::slt_clearTxetEdit()
{
    ui->te_showMsg->clear();
}

void DaqSet::slt_recvCardInfo(QString ip, QString before, QString after)
{
    QString text = "Ip:"+ip+" send:"+before+" recv:"+after;
    ui->te_showMsg->append(text);
}

void DaqSet::slt_receCardTimes(QString Ip, int total, int failed)
{
    for(int i=0;i<ui->table_Msg->rowCount();++i)
    {
        if(Ip == ui->table_Msg->item(i,2)->text())
        {
            if(m_mapCardRunTime.contains(Ip) &&
                    m_mapCardRunTime.value(Ip).bJudge)
            {
                QDateTime beginTime = QDateTime::fromString(m_mapCardRunTime.value(Ip).strBeginTime,"yyyyMMddhhmmss");
                int second = QDateTime::currentDateTime().secsTo(beginTime);
                ui->table_Msg->setItem(i,4,new QTableWidgetItem(QString::number(second)));
            }
            ui->table_Msg->setItem(i,5,new QTableWidgetItem(QString::number(total)));
            ui->table_Msg->setItem(i,6,new QTableWidgetItem(QString::number(failed)));
        }
    }
}

void DaqSet::on_rb_Moxa_clicked()
{
    if(ui->rb_Moxa->isChecked())
    {
        ui->rb_485->setEnabled(false);
        ui->rb_net->setChecked(true);
    }
}

void DaqSet::on_rb_smacq_clicked()
{
    if(ui->rb_smacq->isChecked())
    {
        ui->rb_485->setEnabled(true);
    }
}

void DaqSet::on_comB_cardType_currentIndexChanged(const QString &arg1)
{
    if(arg1.trimmed() == "1211")
    {
        ui->sb_pass->setRange(0,15);
    }
    else if(arg1.trimmed() == "1240")
    {
        ui->sb_pass->setRange(0,8);
    }
    else if(arg1.trimmed() == "4510")
    {
        ui->sb_pass->setRange(0,15);
    }
    else if(arg1.trimmed() == "思迈科华")
    {
        ui->sb_pass->setRange(0,8);
    }
}

bool DaqSet::judgeSettingInfo()
{
    if(ui->rb_Moxa->isChecked() == false
            && ui->rb_smacq->isChecked() == false)
    {
        QMessageBox::information(this,"提示","请选择数采卡类型。","确定");
        return false;
    }
    if(ui->rb_485->isChecked() == false
            && ui->rb_net->isChecked() == false)
    {
        QMessageBox::information(this,"提示","请选择连接数采卡类型。","确定");
        return false;
    }
    if(ui->le_timeInterval->text().trimmed().isEmpty())
    {
        QMessageBox::information(this,"提示","请输入测试时间间隔。","确定");
        return false;
    }

    if(ui->le_ip1->text().trimmed().isEmpty() ||
            ui->le_ip2->text().trimmed().isEmpty() ||
            ui->le_ip3->text().trimmed().isEmpty() ||
            ui->le_ip4->text().trimmed().isEmpty())
    {
        QMessageBox::information(this,"提示","请输入正确Ip地址。","确定");
        return false;
    }

    for(int i=0;i<ui->table_Msg->rowCount();++i)
    {
        if(ui->table_Msg->item(i,2)->text() == getIpAddr())
        {
            QMessageBox::information(this,"提示","该ip地址已存在，请重新输入。","确定");
            return false;
        }
    }

    return true;
}

QString DaqSet::getIpAddr()
{
    QString ip = ui->le_ip1->text().trimmed()+"."+ui->le_ip2->text().trimmed()+"."
            +ui->le_ip3->text().trimmed()+"."+ui->le_ip4->text().trimmed();
    return ip;
}
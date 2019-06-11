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
    connect(&m_clearTextTimer,SIGNAL(timeout()),this,SLOT(slt_clearTextEdit()));
    m_clearTextTimer.start(1000);
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
        num1211 = 1;
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
    ui->table_Info->setItem(row,4,new QTableWidgetItem());
    ui->table_Info->setItem(row,5,new QTableWidgetItem());
    ui->table_Info->setItem(row,6,new QTableWidgetItem());
}

void MultipleSet::on_pb_start_clicked()
{
    if(ui->table_Info->currentRow() != -1)
    {
        QString ip = ui->table_Info->item(ui->table_Info->currentRow(),1)->text();
        int nDoCount = ui->table_Info->item(ui->table_Info->currentRow(),2)->text().toInt();
        int nAiCount = ui->table_Info->item(ui->table_Info->currentRow(),3)->text().toInt();
        if(m_mapDeviceRunTime.contains(ip) && m_mapDeviceRunTime.value(ip).bJudge == true)
        {
            QMessageBox::information(this,"提示","请勿重复点击开始测试。","确定");
            return;
        }
        if(m_mapDeviceRunTime.contains(ip) && m_mapDeviceRunTime.value(ip).bJudge == false)
        {
            if(m_mapDeviceObject.contains(ip))
            {
                m_mapDeviceRunTime[ip].bJudge = true;
                m_mapDeviceObject.value(ip)->start();
            }
            return;
        }
        RunTime time;
        time.bJudge = true;
        time.strBeginTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        m_mapDeviceRunTime.insert(ip,time);
        IODevice* device = new Device;
        connect(device,SIGNAL(sig_connectfailed(QString)),this,SLOT(slt_recvConnectFailed(QString)),Qt::QueuedConnection);
        connect(device,SIGNAL(sig_IOCount(QString,int,int)),this,SLOT(slt_receDeviceTimes(QString,int,int)),Qt::QueuedConnection);
        connect(device,SIGNAL(sig_IObuf(QString,QByteArray,QByteArray)),this,SLOT(slt_recvDeviceInfo(QString,QByteArray,QByteArray)),Qt::QueuedConnection);
        device->setDeviceCount(nDoCount,nAiCount);
        device->Open(ip);
        m_mapDeviceObject.insert(ip,device);
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
        if(m_mapDeviceRunTime.contains(ip) && m_mapDeviceRunTime[ip].bJudge == true)
        {
            m_mapDeviceRunTime[ip].bJudge = false;
            m_mapDeviceObject.value(ip)->Stop();
        }
    }
    else
    {
        QMessageBox::information(this,"提示","点击停止前请先选中表格中某行数据。","确定");
    }
}

void MultipleSet::slt_clearTextEdit()
{
//    qDebug()<<ui->te_showMsg->toPlainText().toLatin1().size();
    if(ui->te_showMsg->toPlainText().toLatin1().size() > 10000*5)
        ui->te_showMsg->clear();
}

void MultipleSet::slt_recvDeviceInfo(const QString &ip, const QByteArray &before, const QByteArray &after)
{
    QString text = "Ip:"+ip+" send:"+(QString)before.toHex()+" recv:"+(QString)after.toHex();
    ui->te_showMsg->append((text));
}

void MultipleSet::slt_receDeviceTimes(const QString &Ip,const int& total,const int &failed)
{
    //    qDebug()<<"total"<<total<<"failed"<<failed<<Ip;
    for(int i=0;i<ui->table_Info->rowCount();++i)
    {
        if(Ip == ui->table_Info->item(i,1)->text())
        {
            if(m_mapDeviceRunTime.contains(Ip) &&
                    m_mapDeviceRunTime.value(Ip).bJudge)
            {
                QDateTime beginTime = QDateTime::fromString(m_mapDeviceRunTime.value(Ip).strBeginTime,"yyyyMMddhhmmss");
                int second = beginTime.secsTo(QDateTime::currentDateTime());
                ui->table_Info->item(i,4)->setText(QString::number(second));
                ui->table_Info->item(i,5)->setText(QString::number(total));
                ui->table_Info->item(i,6)->setText(QString::number(failed));
            }
        }
    }
}

void MultipleSet::slt_recvConnectFailed(QString ip)
{
    if(m_mapDeviceRunTime.contains(ip))
        m_mapDeviceRunTime[ip].bJudge = false;
    QMessageBox::information(this,"提示","ip:"+ip+"设备连接失败。","确定");
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


void MultipleSet::on_pb_deleteSet_clicked()
{
    if(ui->table_Info->currentRow() != -1)
    {
        QString ip = ui->table_Info->item(ui->table_Info->currentRow(),1)->text();
        if(m_mapDeviceRunTime.contains(ip))
        {
            m_mapDeviceRunTime.remove(ip);
            IODevice* device = m_mapDeviceObject.take(ip);
            device->Stop();
            device->Close();
            device->deleteLater();
            device = NULL;
        }
        ui->table_Info->removeRow(ui->table_Info->currentRow());
    }
    else
    {
        QMessageBox::information(this,"提示","点击停止前请先选中表格中某行数据。","确定");
    }
}

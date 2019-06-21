#include "daqset.h"
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
    initPage();
}

DaqSet::~DaqSet()
{
    for(auto it=m_mapIOCardObject.begin();it!=m_mapIOCardObject.end();)
    {
        it++;
        QString key;
        IOCard* card = m_mapIOCardObject.take(key);
        delete card;
        card = NULL;
    }
    delete ui;
}

void DaqSet::on_pb_saveInfo_clicked()
{
    //保存输入配置信息到表格中
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
    ui->table_Msg->setItem(row,3,new QTableWidgetItem(ui->le_port->text().trimmed()));
    ui->table_Msg->setItem(row,4,new QTableWidgetItem(ui->le_timeInterval->text().trimmed()));
    ui->table_Msg->setItem(row,5,new QTableWidgetItem(QString::number(ui->sb_pass->value())));
    ui->table_Msg->horizontalHeader()->show();
}

void DaqSet::on_pb_start_clicked()
{
    if(ui->table_Msg->currentRow() != -1)
    {
        CardInfo cInfo = getCardInfo();
        if(m_mapCardRunTime.contains(cInfo.strIp+cInfo.strSuffix)
                && m_mapCardRunTime.value(cInfo.strIp+cInfo.strSuffix).bJudge == true)
        {
            QMessageBox::information(this,"提示","请勿重复点击开始测试。","确定");
            return;
        }
        if(m_mapCardRunTime.contains(cInfo.strIp+cInfo.strSuffix)
                && m_mapCardRunTime.value(cInfo.strIp+cInfo.strSuffix).bJudge == false)
        {
            if(m_mapIOCardObject.contains(cInfo.strIp+cInfo.strSuffix))
            {
                m_mapCardRunTime[cInfo.strIp+cInfo.strSuffix].bJudge = true;
                m_mapIOCardObject.value(cInfo.strIp+cInfo.strSuffix)->startThread();
            }
            return;
        }
        RunTime time;
        time.bJudge = true;
        time.strBeginTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        if(!cInfo.strSuffix.isEmpty())
        {
            m_mapCardRunTime.insert(cInfo.strIp+cInfo.strSuffix,time);
        }
        else
        {
            m_mapCardRunTime.insert(cInfo.strIp,time);
        }
        IOCard* card = getIOCardObject(cInfo.strCardType);
        if(card == NULL)
            return;
        connect(card,SIGNAL(sig_sendRecv(QString,QByteArray,QByteArray)),this,SLOT(slt_recvCardInfo(QString,QByteArray,QByteArray)),Qt::QueuedConnection);
        connect(card,SIGNAL(sig_statisticsCounts(QString,int,int)),this,SLOT(slt_receCardTimes(QString,int,int)),Qt::QueuedConnection);
        connect(card,SIGNAL(sig_connectFailed(QString)),this,SLOT(slt_recvConnectFailed(QString)),Qt::QueuedConnection);
        card->setBitCount(cInfo.nBitCount);
        card->setMutex(&m_mutex);
        card->setTimeInterval(cInfo.nTimeInterval);
        card->setDeviceAddress(cInfo.strDeviceAddr.toInt());
        card->Open(cInfo.strIp,cInfo.nPort);
        card->startThread();
        if(!cInfo.strDeviceAddr.isEmpty())
        {
            m_mapIOCardObject.insert(cInfo.strIp+cInfo.strSuffix,card);
        }
        else
        {
            m_mapIOCardObject.insert(cInfo.strIp,card);
        }
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
        QString ip = ui->table_Msg->item(ui->table_Msg->currentRow(),2)->text();
        if(m_mapCardRunTime.contains(ip) && m_mapCardRunTime[ip].bJudge == true)
        {
            m_mapCardRunTime[ip].bJudge = false;
            if(m_mapIOCardObject.contains(ip))
                m_mapIOCardObject.value(ip)->stopThread();
        }
    }
    else
    {
        QMessageBox::information(this,"提示","点击停止前请先选中表格中某行数据。","确定");
    }
}

void DaqSet::slt_clearTxetEdit()
{
    if(ui->te_showMsg->toPlainText().toLatin1().size() > 10000*5)
        ui->te_showMsg->clear();
}

void DaqSet::slt_recvCardInfo(QString ip, QByteArray before, QByteArray after)
{
    QString text ="time:"+QDateTime::currentDateTime().toString("yyyyMMdd hh:mm:ss:zzz")+" Ip:"+ip+" send:"+(QString)before.toHex()+" recv:"+(QString)after.toHex();
//    qDebug()<<text;
        ui->te_showMsg->append((text));
}

void DaqSet::slt_receCardTimes(QString Ip, int total, int failed)
{
    //    qDebug()<<Ip;
    for(int i=0;i<ui->table_Msg->rowCount();++i)
    {
        if(Ip == ui->table_Msg->item(i,2)->text())
        {
            if(m_mapCardRunTime.contains(Ip) &&
                    m_mapCardRunTime.value(Ip).bJudge)
            {
                QDateTime beginTime = QDateTime::fromString(m_mapCardRunTime.value(Ip).strBeginTime,"yyyyMMddhhmmss");
                int second = beginTime.secsTo(QDateTime::currentDateTime());
                ui->table_Msg->setItem(i,6,new QTableWidgetItem(QString::number(second)));
                ui->table_Msg->setItem(i,7,new QTableWidgetItem(QString::number(total)));
                ui->table_Msg->setItem(i,8,new QTableWidgetItem(QString::number(failed)));
            }
        }
    }
}

void DaqSet::on_rb_Moxa_clicked()
{
    if(ui->rb_Moxa->isChecked())
    {
        ui->rb_485->setEnabled(false);
        ui->rb_net->setChecked(true);
        ui->le_deviceAddr->setEnabled(false);
    }
}

void DaqSet::on_rb_smacq_clicked()
{
    if(ui->rb_smacq->isChecked())
    {
        ui->rb_485->setEnabled(true);
        ui->le_deviceAddr->setEnabled(true);
    }
}

void DaqSet::on_comB_cardType_currentIndexChanged(const QString &arg1)
{
    if(arg1.trimmed() == "1211")
    {
        ui->sb_pass->setRange(0,16);
    }
    else if(arg1.trimmed() == "1240")
    {
        ui->sb_pass->setRange(0,8);
    }
    else if(arg1.trimmed() == "4510")
    {
        ui->sb_pass->setRange(0,16);
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
    if(ui->rb_smacq->isChecked() && ui->le_deviceAddr->text().isEmpty())
    {
        QMessageBox::information(this,"提示","请输入思迈科华对应的数采卡设备地址。","确定");
        return false;
    }
    if(ui->le_port->text().isEmpty())
    {
        QMessageBox::information(this,"提示","请输入正确端口号。","确定");
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
    if(ui->rb_smacq->isChecked() &&
            !ui->le_deviceAddr->text().trimmed().isEmpty())
    {
        ip.append("|"+ui->le_deviceAddr->text().trimmed());
    }
    return ip;
}

void DaqSet::initPage()
{
    QButtonGroup *group1 = new QButtonGroup(this);
    group1->addButton(ui->rb_Moxa);
    group1->addButton(ui->rb_smacq);

    QButtonGroup *group2 = new QButtonGroup(this);
    group2->addButton(ui->rb_485);
    group2->addButton(ui->rb_net);

    connect(&m_clearTextTimer,SIGNAL(timeout()),this,SLOT(slt_clearTxetEdit()));
    m_clearTextTimer.start(1000);

    ui->le_ip1->setValidator(new QIntValidator(0,255,ui->le_ip1));
    ui->le_ip2->setValidator(new QIntValidator(0,255,ui->le_ip2));
    ui->le_ip3->setValidator(new QIntValidator(0,255,ui->le_ip3));
    ui->le_ip4->setValidator(new QIntValidator(0,255,ui->le_ip4));
    ui->le_timeInterval->setValidator(new QIntValidator(0,999999,ui->le_timeInterval));
    ui->le_port->setValidator(new QIntValidator(0,999999,ui->le_port));
    ui->table_Msg->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

IOCard *DaqSet::getIOCardObject(QString cardType)
{
    IOCard* card = NULL;
    if(cardType == "1211")
    {
        card = new E1211;
    }
    else if(cardType == "1240")
    {
        card = new E1240;
    }
    else if(cardType == "思迈科华AI")
    {
        card = new SmacqAI;
    }
    else if(cardType == "思迈科华DO")
    {
        card = new SmacqDO;
    }
    else if(cardType == "思迈科华")
    {
        card = new ScamqAIDO;
    }
    else if(cardType == "宇电")
    {
        card = new YD516P;
    }

    return card;
}

CardInfo DaqSet::getCardInfo()
{
    CardInfo info;;
    info.strIp = ui->table_Msg->item(ui->table_Msg->currentRow(),2)->text();
    if(info.strIp.contains("|"))
    {
        QStringList list = info.strIp.split("|");
        info.strIp = list.at(0);
        info.strDeviceAddr = list.at(1);
        info.strSuffix = "|"+info.strDeviceAddr;
    }
    info.strCardType = ui->table_Msg->item(ui->table_Msg->currentRow(),1)->text();
    info.nPort = ui->table_Msg->item(ui->table_Msg->currentRow(),3)->text().toInt();
    info.nTimeInterval = ui->table_Msg->item(ui->table_Msg->currentRow(),4)->text().toInt();
    info.nBitCount = ui->table_Msg->item(ui->table_Msg->currentRow(),5)->text().toInt();
    return info;
}

void DaqSet::on_pb_deleteInfo_clicked()
{
    if(ui->table_Msg->currentRow() != -1)
    {
        QString ip = ui->table_Msg->item(ui->table_Msg->currentRow(),2)->text();
        if(m_mapCardRunTime.contains(ip))
        {
            m_mapCardRunTime.remove(ip);
            if(m_mapIOCardObject.contains(ip))
            {
                IOCard* card = m_mapIOCardObject.take(ip);
                card->stopThread();
                card->closeThread();
                delete card;
                //            card->deleteLater();
                card = NULL;
            }
        }
        ui->table_Msg->removeRow(ui->table_Msg->currentRow());
    }
    else
    {
        QMessageBox::information(this,"提示","点击停止前请先选中表格中某行数据。","确定");
    }
}

void DaqSet::slt_recvConnectFailed(QString ip)
{

    if(QMessageBox::information(this,"提示","ip:"+ip+"连接失败，是否重新连接？","是","否")==0)
    {
        if(m_mapIOCardObject.contains(ip))
        {
            m_mapIOCardObject.value(ip)->reConnect(ip);
        }
        if(m_mapCardRunTime.contains(ip))
        {
            m_mapCardRunTime[ip].bJudge = false;
        }
        //        on_pb_start_clicked();
    }
    else
    {
        if(m_mapCardRunTime.contains(ip))
        {
            m_mapCardRunTime[ip].bJudge = false;
        }
    }
}

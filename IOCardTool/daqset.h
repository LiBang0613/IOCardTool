#ifndef DAQSET_H
#define DAQSET_H

#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QDateTime>
#include <QMap>
#include "iocard.h"
#include "e1240.h"

struct DataCardInfo
{
    int    nCardType; 			//数采卡类型，MOXAMODE:moxa; SMACQMODE:思迈科华;
    int  nConnectType;		   //网络连接类型，485TYPE:485类型; NETTYPE:网口类型
    QString strType;			//数采卡型号
    int   nTimeInterval;		//时间间隔
    int   nPass;				//读取的通道数量
    int   nDeviceAddr;       //设备地址
    QString strIp;            //数采卡的ip地址
};

struct RunTime
{
    bool bJudge;            //开始测试或者停止的标志
    QString strBeginTime;   //开始测试的时间
};


namespace Ui {
class DaqSet;
}

class DaqSet : public QWidget
{
    Q_OBJECT

public:
    explicit DaqSet(QWidget *parent = 0);
    ~DaqSet();

private slots:
    void on_pb_saveInfo_clicked();

    void on_pb_start_clicked();

    void on_pb_stop_clicked();

    void slt_clearTxetEdit();

    //接收数采卡对象的发送和接收到的数据，显示在文本区域中。
    void slt_recvCardInfo(QString ip,QByteArray before,QByteArray after);

    //接收数采卡对象的发送命令的总次数和失败次数的槽函数
    void slt_receCardTimes(QString Ip,int total,int failed);

    void on_rb_Moxa_clicked();

    void on_rb_smacq_clicked();

    void on_comB_cardType_currentIndexChanged(const QString &arg1);

private:
    bool judgeSettingInfo();
    QString getIpAddr();
    void initPage();

private:
    Ui::DaqSet *ui;
    QTimer m_clearTextTimer;                    //清除文本框信息的定时器，信息很多会导致内存上升。
    QMap<QString,RunTime> m_mapCardRunTime;     //记录数采卡开始的时间，用于更新数采卡连接总时间。
    QMap<QString,IOCard*>m_mapIOCardObject;   //保存数采卡对象，key=ip，
};

#endif // DAQSET_H

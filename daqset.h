#ifndef DAQSET_H
#define DAQSET_H

#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QDateTime>
#include <QMap>

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

    void slt_recvCardInfo(QString ip,QString before,QString after);

    void slt_receCardTimes(QString Ip,int total,int failed);

    void on_rb_Moxa_clicked();

    void on_rb_smacq_clicked();


    void on_comB_cardType_currentIndexChanged(const QString &arg1);

private:
    Ui::DaqSet *ui;
    QTimer m_clearTextTimer;                    //清除文本框信息的定时器，信息很多会导致内存上升。
    QMap<QString,QString> m_mapCardRunTime;     //记录数采卡开始的时间，用于更新数采卡连接总时间。
//    QMap<QString,QObject*>m_test;
};

#endif // DAQSET_H

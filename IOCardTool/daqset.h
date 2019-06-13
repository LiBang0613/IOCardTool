#ifndef DAQSET_H
#define DAQSET_H

#include <QWidget>
#include <QTimer>
#include <QMessageBox>
#include <QDateTime>
#include <QMap>
#include "iocard.h"
#include "e1240.h"
#include "e1211.h"
#include "smacqdo.h"
#include "smacqai.h"
#include "scamqaido.h"

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
    //保存输入的数采卡信息并展示在表格中。
    void on_pb_saveInfo_clicked();

    //开始测试选中的数采卡信息
    void on_pb_start_clicked();

    //暂停当前选中运行的数采卡
    void on_pb_stop_clicked();

    //定时清空文本域中的文本，避免内存过大。
    void slt_clearTxetEdit();

    //接收数采卡对象的发送和接收到的数据，显示在文本区域中。
    void slt_recvCardInfo(QString ip,QByteArray before,QByteArray after);

    //接收数采卡对象的发送命令的总次数和失败次数的槽函数
    void slt_receCardTimes(QString Ip,int total,int failed);

    //根据moxa单选按钮禁用485连接方式
    void on_rb_Moxa_clicked();

    //根据smacq单选按钮选中解除485连接禁用状态
    void on_rb_smacq_clicked();

    //根据数采卡类型设置读取通道数的范围
    void on_comB_cardType_currentIndexChanged(const QString &arg1);

    //删除数采卡配置信息及数采卡对象指针。
    void on_pb_deleteInfo_clicked();

    //接收数采卡发送连接失败的信号的槽函数，并根据提示是否进行重新连接。
    void slt_recvConnectFailed(QString ip);

private:
    //判断输入信息是否正确
    bool judgeSettingInfo();
    //根据四个文本框获取ip地址
    QString getIpAddr();
    //初始化页面资源
    void initPage();

private:
    Ui::DaqSet *ui;
    QTimer m_clearTextTimer;                    //清除文本框信息的定时器，信息很多会导致内存上升。
    QMap<QString,RunTime> m_mapCardRunTime;     //记录数采卡开始的时间，用于更新数采卡连接总时间。
    QMap<QString,IOCard*>m_mapIOCardObject;   //保存数采卡对象，key=ip，
    QMutex m_mutex;
};

#endif // DAQSET_H

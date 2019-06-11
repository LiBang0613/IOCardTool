#ifndef IODEVICE_H
#define IODEVICE_H

#include "iocard_global.h"
#include <QObject>

class IOCARDSHARED_EXPORT IODevice : public QObject
{
    Q_OBJECT
//public:
//    IODevice();
//    virtual ~IODevice();

public:
    // 连接设备
    virtual bool Open(QString strIP) = 0;
    // 关闭设备
    virtual bool Close() = 0;
    // 退出通讯线程
    virtual bool Stop() = 0;
    // 设置设备中IO模块的数量
    virtual void setDeviceCount(int nDO, int nAI) = 0; //

    // 重新连接socket
    virtual void reConnect(QString strIP, uint nPort = 520) = 0;
    // 重启通讯线程
    virtual void start() = 0;
signals:
    // 发送接收到的IO设备 发送总次数和失败次数 的信号
    void sig_IOCount(QString strIP, int nTotalCount, int nFailedCount);
    // 发送接收到的IO设备 发送的数据和接收的数据 的信号
    void sig_IObuf(QString strIP, QByteArray bufSend, QByteArray bufRcv);
    // 发送设备连接失败的信号
    void sig_connectfailed(QString);
protected slots:
    // 接收IO设备 发送总次数和失败次数 的槽函数
    virtual void slt_IOCount(QString strIP, int nTotalCount, int nFailedCount) = 0;
    // 接收IO设备 发送和接收 的槽函数
    virtual void slt_IObuf(QString strIP, QByteArray bufSend, QByteArray bufRcv) = 0;

    // 接收IO设备连接失败信号的槽函数
    virtual void slt_connectedfailed(QString strIP) = 0;
};

#endif // IODEVICE_H

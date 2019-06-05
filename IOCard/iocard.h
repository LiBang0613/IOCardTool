#ifndef IOCARD_H
#define IOCARD_H

#include "iocard_global.h"
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

class IOCARDSHARED_EXPORT IOCard : public QObject
{
    Q_OBJECT
public:
    IOCard();
    virtual ~IOCard();
public:
    //连接指定Ip地址
    bool Open(QString strIp,uint nPort = 502);
    //设置读写寄存器 /线圈数量
    void setBitCount(int nCount);
    //启动通讯线程
    void startThread();
    //停止通讯线程
    void stopThread();

    void setTimeInterval(int second);
protected:
    //虚函数，子类实现
    virtual void Process() = 0;

protected slots:
    void slt_tcpConnected();
    void slt_recvSocketState(QAbstractSocket::SocketState state);
    virtual void slt_readyRead() = 0;
    void slt_cycleSendInfo();
    void slt_tcpDisConnected();

signals:
    //统计数据信号，发送多少次命令，失败多少次
    void sig_statisticsCounts(QString,int,int);
    //发送内容信号
    void sig_sendRecv(QString,QByteArray,QByteArray);

protected:
    QThread *m_pThread;
    QTcpSocket m_qTcpSocket;
    QString m_strIp;
    int m_nBitCount;
    int m_nSendTimes;
    int m_nFailedTimes;
    int m_nTimeInterval;
    QTimer m_cycleTimer;
};

#endif // IOCARD_H

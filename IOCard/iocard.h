#ifndef IOCARD_H
#define IOCARD_H

#include "iocard_global.h"
#include <QObject>
#include <QTcpSocket>
#include <QThread>

class IOCARDSHARED_EXPORT IOCard : public QObject
{
    Q_OBJECT
public:
    IOCard();
    ~IOCard();
public:
    //连接指定Ip地址
    bool Open(QString strIp,uint nPort = 502);
    //设置读写寄存器 /线圈数量
    void setBitCount(int nCount);
    //调用这个函数进行socket连接
    bool connectCard(QTcpSocket& socket,QString strIp,uint nPort);
protected:
    //虚函数，子类实现
    virtual void Process() = 0;
    //启动通讯线程
    void startThread();
    //停止通讯线程
    void stopThread();

protected slots:
    void slt_tcpConnected();
    void slt_recvSocketState(QAbstractSocket::SocketState state);
    virtual void slt_readyRead() = 0;

signals:
    //统计数据信号，发送多少次命令，失败多少次
    void sig_statisticsCounts(QString,QByteArray,QByteArray);
    //发送内容信号
    void sig_sendRecv(QString,QByteArray,QByteArray);

protected:
    QThread *m_pThread;
    QTcpSocket m_qTcpSocket;
    int m_nBitCount;
    int m_nSendTimes;
    int m_nFailedTimes;
};

#endif // IOCARD_H

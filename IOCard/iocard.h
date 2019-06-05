#ifndef IOCARD_H
#define IOCARD_H

#include "iocard_global.h"
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

enum WriteState
{
     wsIni = 1
    ,wsReady = 2
    ,wsEnd = 3
};
struct CtrlDO
{
    unsigned short	wID;		// 组ID标识
    int		nRegAddr;
    int		nRegCount;

    unsigned short	wCmd;		// 命令值
    WriteState	wState;		// 写操作-状态

public:
    CtrlDO()
    {
        wID			= 0;		// 组ID标识
        nRegAddr	= 0;
        nRegCount	= 0;

        wCmd		= 0;		// 命令值
        wState		= wsEnd;	// 写操作状态标记
    }
};
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
    virtual void setBitCount(int nCount);
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
    void slt_tcpDisConnected();
    virtual void slt_readyRead() = 0;
    void slt_ProThread();
    void slt_newSocket();
    void slt_stopThread();

signals:
    //统计数据信号，发送多少次命令，失败多少次
    void sig_statisticsCounts(QString,int,int);
    //发送内容信号
    void sig_sendRecv(QString,QByteArray,QByteArray);

    void sig_newSocket();

    void sig_stopThread();

protected:
    QThread *m_pThread;
    QTcpSocket *m_qTcpSocket;
    QString m_strIp;
    int m_nBitCount;
    int m_nSendTimes;
    int m_nFailedTimes;
    int m_nTimeInterval;
    QTimer m_cycleTimer;
public:
    QByteArray m_sendArray;

private:
    bool m_bNewSocket;

};

#endif // IOCARD_H

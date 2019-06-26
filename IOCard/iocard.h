#ifndef IOCARD_H
#define IOCARD_H

#include "iocard_global.h"
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>
#include "logexport.h"

enum WriteState
{
    wsIni = 1
    ,wsReady = 2
    ,wsEnd = 3
};
struct CtrlDO
{
    unsigned short	wID;		// 组ID标识
    int		nRegAddr;           // 线圈起始位置
    int		nRegCount;          // 可控的线圈数量

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
    //重连
    void reConnect(QString strIP, uint nPort = 502);
    //设置读写寄存器 /线圈数量
    virtual void setBitCount(int nCount);
    //启动通讯线程
    void startThread();
    //停止通讯线程
    void stopThread();

    void setTimeInterval(int second);

    void closeThread();

    void setDeviceAddress(int addr);

    void setMutex(QMutex* mutex);
public:
    int m_nSendTimes;
    int m_nFailedTimes;
protected:
    //虚函数，子类实现
    virtual void Process() = 0;

protected slots:
    void slt_tcpDisConnected();
    virtual void slt_readyRead() = 0;
    void slt_ProThread();
    void slt_newSocket();
    void slt_stopThread();

    void slt_reConnect(QString strIP, uint nPort = 502);
signals:
    //统计数据信号，发送多少次命令，失败多少次
    void sig_statisticsCounts(QString,int,int);
    //发送内容信号
    void sig_sendRecv(QString,QByteArray,QByteArray);

    // socket准备信号，发送后准备设备连接的socket
    void sig_newSocket();

    // socket断开信号，断开socket连接
    void sig_stopThread();

    // 线程函数开始执行信号
    void sig_operate();

    // 设备连接失败信号
    void sig_connectFailed(QString);

    // 重新连接设备信号
    void sig_reConnect(QString strIP, uint nPort = 502);
protected:
    QThread *m_pThread;
    QTcpSocket *m_qTcpSocket;
    QString m_strIp;            // 设备IP地址
    int m_nPort;
    int m_nBitCount;            // 可控的线圈/寄存器数量
    int m_nTimeInterval;        // 命令发送时间间隔，单位：ms
    int m_nSmacqAddr;           //思迈科华设备地址。
    QMutex *m_mutex;
    QString m_strDeviceAddr;

public:
    QByteArray m_sendArray;     // 保存设备发送的命令

private:
    bool m_bNewSocket;          // socket是否已存在
    bool m_bExitThread;         // 是否退出线程函数  true - 退出, false - 不退出

    QString getIpAddr();


};

#endif // IOCARD_H

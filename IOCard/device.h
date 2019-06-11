#ifndef DEVICE_H
#define DEVICE_H

#include "iodevice.h"
#include "e1211.h"
#include "e1240.h"
class IOCARDSHARED_EXPORT Device : public IODevice
{
public:
    Device();
    ~Device();

public:
    // 连接设备
    virtual bool Open(QString strIP);
    // 关闭设备
    virtual bool Close();
    // 退出通讯线程
    virtual bool Stop();
    // 设置设备中IO模块的数量
    virtual void setDeviceCount(int nDO, int nAI); //

    // 重新连接socket
    virtual void reConnect(QString strIP, uint nPort = 520);
    // 重启通讯线程
    virtual void start();
protected slots:
    // 接收IO设备 发送总次数和失败次数 的槽函数
    virtual void slt_IOCount(QString strIP, int nTotalCount, int nFailedCount);
    // 接收IO设备 发送和接收 的槽函数
    virtual void slt_IObuf(QString strIP, QByteArray bufSend, QByteArray bufRcv);
    // 接收IO设备连接失败信号的槽函数
    virtual void slt_connectedfailed(QString strIP);
private:
    // 用于获取其他模块的IP地址
    QString GetOffsetIP(int nOffset, QString strIP);

public:
    int m_nCountDO; // DO 模块数量
    int m_nCountAI; // AI 模块数量

    bool m_bSendConnectedFailed; // 是否已发送连接失败信号，保证一个设备只发送一次 true - 已发送，false - 未发送
private:
    QVector<E1211*> m_vctE1211; // 保存生成的DO模块设备地址
    QVector<E1240*> m_vctE1240; // 保存生成的AI模块设备地址

    int m_nTotalCount;      // 总发送次数
    int m_nFailedCount;     // 总失败次数
    QString m_strDeviceIp;  // 设备首个IP地址
};

#endif // DEVICE_H

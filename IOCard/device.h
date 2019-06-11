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
    virtual bool Stop();
    // 设置设备中IO模块的数量
    virtual void setDeviceCount(int nDO, int nAI); //

    virtual void start();
protected slots:
    virtual void slt_IOCount(QString strIP, int nTotalCount, int nFailedCount);
    virtual void slt_IObuf(QString strIP, QByteArray bufSend, QByteArray bufRcv);

private:
    QString GetOffsetIP(int nOffset, QString strIP);

public:
    int m_nCountDO;
    int m_nCountAI;

private:
    QVector<E1211*> m_vctE1211;
    QVector<E1240*> m_vctE1240;

    int m_nTotalCount;
    int m_nFailedCount;
    QString m_strDeviceIp;
};

#endif // DEVICE_H

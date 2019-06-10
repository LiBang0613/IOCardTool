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
    // 设置设备中IO模块的数量
    virtual void setDeviceCount(int nDO, int nAI) = 0; //

    virtual void start() = 0;
signals:
    void sig_IOCount(QString strIP, int nTotalCount, int nFailedCount);
    void sig_IObuf(QString strIP, QByteArray bufSend, QByteArray bufRcv);

    void sig_connectfailed(QString);
protected slots:
    virtual void slt_IOCount(QString strIP, int nTotalCount, int nFailedCount) = 0;
    virtual void slt_IObuf(QString strIP, QByteArray bufSend, QByteArray bufRcv) = 0;
};

#endif // IODEVICE_H

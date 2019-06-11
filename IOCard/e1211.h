#ifndef E1211_H
#define E1211_H
#include "iocard.h"


class IOCARDSHARED_EXPORT E1211 : public IOCard
{
    Q_OBJECT
public:
    E1211();
    ~E1211();


    //设置读写寄存器 /线圈数量
    virtual void setBitCount(int nCount);
protected:
    // 获取读线圈命令
    bool Query_Read(QByteArray& sendBuf, int& nLen);
    // 解析读线圈返回值
    bool Response_Read(QByteArray recvBuf,int nLen);
    // 获取写线圈命令
    bool Query_Write(QByteArray& sendBuf, int& nLen);
    // 解析写线圈返回值
    bool Response_Write(QByteArray recvBuf, int nLen);

    // 线程函数
    void Process();

    // 设置线圈位状态
    void WriteCmdDO();

protected slots:
    void slt_readyRead();

private:
    bool m_bMarkState;  // TRUE - 上次写线圈为开，FALSE - 上次写线圈为关
    bool m_bWriteDO;    // 1211设备是否要执行写线圈命令
    CtrlDO m_ctrlDO;    // 写线圈的参数
};

#endif // E1211_H

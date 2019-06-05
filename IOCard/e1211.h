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
    bool Query_Read(QByteArray& sendBuf, int& nLen);
    bool Response_Read(QByteArray recvBuf,int nLen);
    bool Query_Write(QByteArray& sendBuf, int& nLen);
    bool Response_Write(QByteArray recvBuf, int nLen);
    void Process();

    void WriteCmdDO();

protected slots:
    void slt_readyRead();

private:
    bool m_bMarkState;  // TRUE - 上次写线圈为开，FALSE - 上次写线圈为关
    bool m_bWriteDO;
    CtrlDO m_ctrlDO;
};

#endif // E1211_H

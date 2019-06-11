#ifndef E1240_H
#define E1240_H
#include "iocard.h"

class IOCARDSHARED_EXPORT E1240 : public IOCard
{
    Q_OBJECT
public:
    E1240();
    ~E1240();

protected:
    // 获取读寄存器数据命令
    bool Query_Read(QByteArray &sendBuf,int &nLen);

    // 解析数据
    bool Response_Read(QByteArray &recvBuf, int nLen);

    // 线程函数
    void Process();

protected slots:
    void slt_readyRead();
};

#endif // E1240_H

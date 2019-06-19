#ifndef YD516P_H
#define YD516P_H
#include "iocard.h"
#include "iocard_global.h"

class IOCARDSHARED_EXPORT YD516P : public IOCard
{
public:
    YD516P();
    ~YD516P();
protected:
    // 获取读寄存器数据命令
    bool Query_Read(QByteArray &sendBuf,int &nLen);

    // 解析数据
    bool Response_Read(QByteArray &recvBuf, int nLen);

    // 线程函数
    void Process();

    ushort getCheckCode();
protected slots:
    void slt_readyRead();
private:
    int m_nCommand;
};

#endif // YD516P_H

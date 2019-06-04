#ifndef E1211_H
#define E1211_H
#include "iocard.h"


class IOCARDSHARED_EXPORT E1211 : public IOCard
{
    Q_OBJECT
public:
    E1211();
    ~E1211();
protected:
    bool Query_Read(QByteArray& sendBuf, int& nLen);
    bool Response_Read(QByteArray recvBuf,int nLen);
    bool Query_Write(QByteArray& sendBuf,int& nlen);
    bool Response_Write(QByteArray recvBuf, int nLen);
    void Process();
};

#endif // E1211_H

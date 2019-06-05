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
    bool Query_Read(QByteArray &sendBuf,int &nLen);
    bool Response_Read(QByteArray recvBuf, int nLen);

    void Process();

protected slots:
    void slt_readyRead();
private:
    QByteArray m_sendArray;
};

#endif // E1240_H

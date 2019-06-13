#ifndef SMACQAI_H
#define SMACQAI_H
#include "iocard.h"
#include "iocard_global.h"


class IOCARDSHARED_EXPORT SmacqAI : public IOCard
{
public:
    SmacqAI();
    ~SmacqAI();
protected:
    virtual	void	Process();

    ushort	CalCRC(QByteArray&pBuf, int nBufLen);
    bool	Query_ReadAI(QByteArray &pBuf , int& nLen);
    void	Response_ReadAI(QByteArray &pBuf , const int& nLen);
    bool    Query_SetSpeedAI(QByteArray &pBuf,int& nLen);
    void    Response_SetSpeedAI(QByteArray &pBuf , const int& nLen);
protected slots:
    void slt_readyRead();

private:
    bool m_bSetSpeed;
};

#endif // SMACQAI_H

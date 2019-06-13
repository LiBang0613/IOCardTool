#ifndef SMACQDO_H
#define SMACQDO_H
#include "iocard.h"
#include "iocard_global.h"

class IOCARDSHARED_EXPORT SmacqDO : public IOCard
{
public:
    SmacqDO();
    ~SmacqDO();

    virtual void setBitCount(int nCount);

protected:
    virtual	void	Process();

    ushort	CalCRC(QByteArray&pBuf, int nBufLen);
    bool	Queray_WriteDO(QByteArray &pBuf ,int& nLen);
    bool	Query_ReadDO(QByteArray &pBuf ,int& nLen);
    void	Response_WriteDO(QByteArray &pBuf , const int& nLen);
    void	Response_ReadDO(QByteArray &pBuf ,const int& nLen);
    // 设置线圈位状态
    void WriteCmdDO();
protected slots:
    void slt_readyRead();
protected:
    ushort	m_wDOState;
    uint	m_unCount;
private:
    bool m_bMarkState;  // TRUE - 上次写线圈为开，FALSE - 上次写线圈为关
    bool m_bWriteDO;    // 1211设备是否要执行写线圈命令
    CtrlDO m_ctrlDO;
};

#endif // SMACQDO_H

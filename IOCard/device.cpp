#include "device.h"

Device::Device()
{
    m_nCountAI = 0;
    m_nCountDO = 0;

    m_nTotalCount = 0;
    m_nFailedCount = 0;
}

Device::~Device()
{

}

bool Device::Open(QString strIP)
{
    if(m_nCountAI <=0 && m_nCountDO <= 0)
        return false;

    QVector<QString> vctIP;

    QString strNewIP;
    strNewIP = strIP;
    vctIP.push_back(strIP);
    for(int i = 1; i < m_nCountAI + m_nCountDO; i++)
    {
        strNewIP = GetOffsetIP(1, strNewIP);
        vctIP.push_back(strNewIP);
    }

    for(int i = 0; i < m_nCountAI; i++)
    {
        E1240* card = new E1240;
        card->setBitCount(7);
        card->setTimeInterval(10);
        connect(card, SIGNAL(sig_sendRecv(QString,QByteArray,QByteArray)), this, SLOT(slt_IObuf(QString,QByteArray,QByteArray)), Qt::QueuedConnection);
        connect(card, SIGNAL(sig_statisticsCounts(QString,int,int)), this, SLOT(slt_IOCount(QString,int,int)), Qt::QueuedConnection);
        connect(card, SIGNAL(sig_connectFailed(QString)), this, SIGNAL(sig_connectfailed(QString)), Qt::QueuedConnection);

        card->Open(vctIP[i]);
        card->startThread();
        m_vctE1240.push_back(card);
    }
    for(int i = 0; i < m_nCountDO; i++)
    {
        E1211* card = new E1211;
        card->setBitCount(2);
        card->setTimeInterval(5);
        connect(card, SIGNAL(sig_sendRecv(QString,QByteArray,QByteArray)), this, SLOT(slt_IObuf(QString,QByteArray,QByteArray)), Qt::QueuedConnection);
        connect(card, SIGNAL(sig_statisticsCounts(QString,int,int)), this, SLOT(slt_IOCount(QString,int,int)), Qt::QueuedConnection);
        connect(card, SIGNAL(sig_connectFailed(QString)), this, SIGNAL(sig_connectfailed(QString)), Qt::QueuedConnection);
        card->Open(vctIP[i + m_nCountAI]);
        card->startThread();
        m_vctE1211.push_back(card);
    }

    return true;
}

bool Device::Close()
{
    for(int i = 0; i < m_nCountAI; i++)
        m_vctE1240[i]->stopThread();

    for(int i = 0; i < m_nCountDO; i++)
        m_vctE1211[i]->stopThread();

    return true;
}

void Device::setDeviceCount(int nDO, int nAI)
{
    m_nCountDO = nDO;
    m_nCountAI = nAI;
}

void Device::slt_IOCount(QString strIP, int nTotalCount, int nFailedCount)
{
    m_nTotalCount += nTotalCount;
    m_nFailedCount += nFailedCount;
    emit sig_IOCount(strIP, m_nTotalCount, m_nFailedCount);
}

void Device::slt_IObuf(QString strIP, QByteArray bufSend, QByteArray bufRcv)
{
    emit sig_IObuf(strIP, bufSend, bufRcv);
}

QString Device::GetOffsetIP(int nOffset, QString strIP)
{
    int nIndex = 0;
    int nFind =0;

    while(nIndex < 3)
    {
        nFind = strIP.indexOf(".", nFind + 1);
        nIndex++;
    }

    int nNum = (strIP.mid(nFind + 1)).toInt();

    QString strNewIP;
    strNewIP.sprintf(".%d", ++nNum);
    strNewIP = strIP.mid(0, nFind) + strNewIP;

    return strNewIP;
}

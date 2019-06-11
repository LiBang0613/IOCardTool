#include "device.h"

Device::Device()
{
    m_nCountAI = 0;
    m_nCountDO = 0;

    m_nTotalCount = 0;
    m_nFailedCount = 0;

    m_bSendConnectedFailed = false;
}

Device::~Device()
{
    for(int i = 0; i < m_vctE1240.size(); i++)
    {
        E1240* card = m_vctE1240[i];
        delete card;
        card = NULL;
    }

    for(int i = 0; i < m_vctE1211.size(); i++)
    {
        E1211* card = m_vctE1211[i];
        delete card;
        card = NULL;
    }
    m_vctE1211.clear();
    m_vctE1240.clear();
}

bool Device::Open(QString strIP)
{
    if(m_nCountAI <=0 && m_nCountDO <= 0)
        return false;

     m_strDeviceIp = strIP;
    QVector<QString> vctIP;

    // ------------>> 获取偏移地址 ---------------------
    QString strNewIP;
    strNewIP = strIP;
    vctIP.push_back(strIP);
    for(int i = 1; i < m_nCountAI + m_nCountDO; i++)
    {
        strNewIP = GetOffsetIP(1, strNewIP);
        vctIP.push_back(strNewIP);
    }
    // ------------<< 获取偏移地址 ---------------------

    // ------------>> 连接设备 ------------------------
    for(int i = 0; i < m_nCountAI; i++)
    {
        E1240* card = new E1240;
        card->setBitCount(7);
        card->setTimeInterval(10);
        connect(card, SIGNAL(sig_sendRecv(QString,QByteArray,QByteArray)), this, SLOT(slt_IObuf(QString,QByteArray,QByteArray)), Qt::QueuedConnection);
        connect(card, SIGNAL(sig_statisticsCounts(QString,int,int)), this, SLOT(slt_IOCount(QString,int,int)), Qt::QueuedConnection);
        connect(card, SIGNAL(sig_connectFailed(QString)), this, SLOT(slt_connectedfailed(QString)), Qt::QueuedConnection);

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
        connect(card, SIGNAL(sig_connectFailed(QString)), this, SLOT(slt_connectedfailed(QString)), Qt::QueuedConnection);
        card->Open(vctIP[i + m_nCountAI]);
        card->startThread();
        m_vctE1211.push_back(card);
    }
    // ------------<< 连接设备 ------------------------

    return true;
}

bool Device::Close()
{
    // 退出设备通讯线程
    for(int i = 0; i < m_nCountAI; i++)
    {
        m_vctE1240[i]->closeThread();
    }

    for(int i = 0; i < m_nCountDO; i++)
    {
        m_vctE1211[i]->closeThread();
    }
    logInfo(m_strDeviceIp+"_device","关闭设备通讯线程");
    return true;
}

bool Device::Stop()
{
    // 退出设备通讯线程函数
    for(int i = 0; i < m_nCountAI; i++)
    {
        m_vctE1240[i]->stopThread();
    }

    for(int i = 0; i < m_nCountDO; i++)
    {
        m_vctE1211[i]->stopThread();
    }
    logInfo(m_strDeviceIp+"_device","停止设备通讯线程");
    return true;
}

void Device::setDeviceCount(int nDO, int nAI)
{
    m_nCountDO = nDO;
    m_nCountAI = nAI;
}

void Device::reConnect(QString strIP, uint nPort)
{
    // 获取偏移地址，重新连接设备
    QVector<QString> vctIP;

    QString strNewIP;
    strNewIP = m_strDeviceIp;
    vctIP.push_back(m_strDeviceIp);
    for(int i = 1; i < m_nCountAI + m_nCountDO; i++)
    {
        strNewIP = GetOffsetIP(1, strNewIP);
        vctIP.push_back(strNewIP);
    }

    for(int i = 0; i < m_nCountAI; i++)
    {
        m_vctE1240[i]->reConnect(vctIP[i]);
    }

    for(int i = 0; i < m_nCountDO; i++)
    {
        m_vctE1211[i]->reConnect(vctIP[i+m_nCountAI]);
    }
    m_bSendConnectedFailed = false;
}

// 重启设备线程函数
void Device::start()
{
    if(m_vctE1211.size() <= 0 && m_vctE1240.size() <= 0)
        return ;

    QVector<QString> vctIP;

    QString strNewIP;
    strNewIP = m_strDeviceIp;
    vctIP.push_back(m_strDeviceIp);
    for(int i = 1; i < m_nCountAI + m_nCountDO; i++)
    {
        strNewIP = GetOffsetIP(1, strNewIP);
        vctIP.push_back(strNewIP);
    }

    for(int i = 0; i < m_vctE1240.size(); i++)
    {
        m_vctE1240[i]->startThread();
    }

    for(int i = 0; i < m_vctE1211.size(); i++)
    {
        m_vctE1211[i]->startThread();
    }
    logInfo(m_strDeviceIp+"_device","启动设备通讯线程");
}

void Device::slt_IOCount(QString strIP, int nTotalCount, int nFailedCount)
{
    // ------------>> 计算设备中IO模块总的发送次数和错误次数--------------------
    int nAITotalCount = 0;
    int nAIFailedCount = 0;
    int nDOTotalCount = 0;
    int nDOFailedCOunt = 0;
    for(int i = 0; i < m_nCountAI; i++)
    {
        nAITotalCount = m_vctE1240[i]->m_nSendTimes;
        nAIFailedCount = m_vctE1240[i]->m_nFailedTimes;
    }

    for(int i = 0; i < m_nCountDO; i++)
    {
       nDOTotalCount =  m_vctE1211[i]->m_nSendTimes;
       nDOFailedCOunt = m_vctE1211[i]->m_nFailedTimes;
    }
    m_nTotalCount = nAITotalCount + nDOTotalCount;
    m_nFailedCount = nAIFailedCount + nDOFailedCOunt;
    // ------------<< 计算设备中IO模块总的发送次数和错误次数--------------------

    // 向外部发送次数信息
    emit sig_IOCount(m_strDeviceIp, m_nTotalCount, m_nFailedCount);
}

void Device::slt_IObuf(QString strIP, QByteArray bufSend, QByteArray bufRcv)
{
    emit sig_IObuf(strIP, bufSend, bufRcv);
}

void Device::slt_connectedfailed(QString strIP)
{
    if(!m_bSendConnectedFailed)
        emit sig_connectfailed(m_strDeviceIp);

    m_bSendConnectedFailed = true;
    logError(m_strDeviceIp+"_device","设备接收到连接失败信号");
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

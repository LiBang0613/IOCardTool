#include "smacqai.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


SmacqAI::SmacqAI()
{
    m_bSetSpeed = false;
}

SmacqAI::~SmacqAI()
{

}

void SmacqAI::Process()
{
    QThread::msleep(m_nTimeInterval);
    m_mutex->lock();
    QByteArray sendArray;
    int nSendLen = 0;
    if(!m_bSetSpeed)
    {
        Query_SetSpeedAI(sendArray,nSendLen);
    }
    else
    {
        Query_ReadAI(sendArray,nSendLen);
    }

    //    qDebug()<<"1240"<<QThread::currentThreadId()<<m_sendArray<<m_sendArray.size();
    ushort wCRC = CalCRC(sendArray, nSendLen);
    uchar cLowCRC = wCRC & 0xff;
    uchar cHighCRC = wCRC >> 8;
    sendArray[nSendLen++] = cLowCRC;
    sendArray[nSendLen++] = cHighCRC;

    int nLen = m_qTcpSocket->write(sendArray);
    m_sendArray = sendArray;
    m_qTcpSocket->flush();
    m_nSendTimes++;
    if(nSendLen != nLen)
    {
        qDebug()<<"send error";
        m_nFailedTimes++;
        logError(m_strIp+"|"+QString::number(m_nSmacqAddr)+"_data","1240发送数据错误");
        m_mutex->unlock();
        return;
    }

    if(m_qTcpSocket->waitForReadyRead() == false)
    {
        m_nFailedTimes++;
        logError(m_strIp+"|"+QString::number(m_nSmacqAddr)+"_data","1240发送数据错误");
        m_mutex->unlock();
        return ;
    }

    slt_readyRead();
    m_mutex->unlock();
}

ushort SmacqAI::CalCRC(QByteArray &pBuf, int nBufLen)
{
    ushort wCRC = 0xffff;

    for (int n = 0; n < nBufLen; n++)
    {
        //        qDebug()<<(ushort)pBuf[n];
        wCRC ^= (uchar)pBuf[n];
        ushort wCheck = 0;
        for (int i = 0; i < 8; i++)
        {
            wCheck = wCRC & 1;
            wCRC = wCRC >> 1;
            wCRC = wCRC & 0x7fff;
            if (wCheck == 1)
            {
                wCRC = wCRC ^ 0xa001;
            }

            wCRC = wCRC & 0xffff;
        }
    }

    return wCRC;
}

bool SmacqAI::Query_ReadAI(QByteArray &pBuf, int &nLen)
{
    int i = 0;

    pBuf[i++] = m_nSmacqAddr;//设备地址变量
    pBuf[i++] = 0x04;
    pBuf[i++] = 0x75;
    pBuf[i++] = (uchar)0x95;
    pBuf[i++] = 0x00;
    pBuf[i++] = m_nBitCount;	// 8个AI
    nLen = i;


    return true;
}

void SmacqAI::Response_ReadAI(QByteArray &pBuf, const int &nLen)
{
    // 1. 接收长度校验
    if ( nLen != 3 + 2 + m_nBitCount * 2)
    {
        m_nFailedTimes++;
        qDebug()<<"失败11111"<<pBuf;
        return;
    }
    // 2. 功能码+数据长度校验
    if ( pBuf[0].operator !=(0x01)
         ||  pBuf[1].operator !=(0x04)   //设备地址变量
         ||  pBuf[2].operator !=(m_nBitCount * 2)
         )
    {
        m_nFailedTimes++;
        qDebug()<<"失败"<<pBuf<<m_nSmacqAddr;
        return;	// 错误
    }

    ushort wVal = 0;

    for (int i = 0; i < m_nBitCount; i++)
    {
        uchar ucB1  = (uchar)pBuf[i * 2 + 2 + 1];
        uchar ucB2  = (uchar)pBuf[i * 2 + 2 + 2];
        wVal = 	(ucB1 << 8) | ucB2;
    }
}

bool SmacqAI::Query_SetSpeedAI(QByteArray &pBuf, int &nLen)
{
    int i = 0;

    pBuf[i++] = m_nSmacqAddr;//设备地址变量
    pBuf[i++] = 0x06;
    pBuf[i++] = 0x9c;
    pBuf[i++] = 0xb9;
    pBuf[i++] = 0x00;
    pBuf[i++] = 0x03;
    nLen = i;
    return true;
}

void SmacqAI::Response_SetSpeedAI(QByteArray &pBuf, const int &nLen)
{
    m_bSetSpeed = true;
    qDebug()<<"设置ai速度"<<pBuf<<nLen;
}

void SmacqAI::slt_readyRead()
{
    QByteArray recvBuf;
    recvBuf = m_qTcpSocket->readAll();
    int nRcvLen = recvBuf.size();

    if(!m_bSetSpeed)
    {
        Response_SetSpeedAI(recvBuf,nRcvLen);
    }
    else
    {
        Response_ReadAI(recvBuf, nRcvLen);
    }

    emit sig_statisticsCounts(m_strIp+"|"+QString::number(m_nSmacqAddr),m_nSendTimes,m_nFailedTimes);
    emit sig_sendRecv(m_strIp+" type:思迈科华AI",m_sendArray,recvBuf);
    logInfo(m_strIp+"|"+QString::number(m_nSmacqAddr)+"_data","type:思迈科华AI send:"+(QString)m_sendArray.toHex()+" recv:"+(QString)recvBuf.toHex());
}

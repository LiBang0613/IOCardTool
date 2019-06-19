#include "scamqaido.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

ScamqAIDO::ScamqAIDO()
{
    m_unTime = 0;
    m_wDOState = 0;
    m_unCount = 8;
    m_bMarkState = false;
    m_bWriteDO = false;
    m_bStatus = false;
    m_bSetSpeed = true;
}

ScamqAIDO::~ScamqAIDO()
{

}

void ScamqAIDO::Process()
{
    QThread::msleep(m_nTimeInterval);
    QMutexLocker locker(m_mutex);

    m_unTime += 2;

    QByteArray cSendBuf;
    int nSendLen = 0;
    int n = 0;


    if (m_unTime >= 20)
    {
        m_bStatus = true;
    }

    // 检查是否有写位请求

    if (m_ctrlDO.wState != wsEnd)
    {
        m_bWriteDO = Queray_WriteDO(cSendBuf ,nSendLen);
    }

    if (m_bWriteDO)
    {
        m_bStatus = false;
    }

    if (m_bStatus)
    {
        Query_ReadDO(cSendBuf, nSendLen);
        m_unTime = 0;
    }

    if (!m_bWriteDO && !m_bStatus)
    {
        if(m_bSetSpeed)
        {
            Query_SetSpeedAI(cSendBuf,nSendLen);
        }
        else
        {
            Query_ReadAI(cSendBuf, nSendLen);
        }
    }
    ushort wCRC = CalCRC(cSendBuf, nSendLen);
    char cLowCRC = wCRC & 0xff;
    char cHighCRC = wCRC >> 8;
    cSendBuf[nSendLen++] = cLowCRC;
    cSendBuf[nSendLen++] = cHighCRC;

    int nLen = m_qTcpSocket->write(cSendBuf);
    QDateTime sendTime = QDateTime::currentDateTime();
    m_qTcpSocket->flush();
    m_nSendTimes++;
    if( nSendLen != nLen)	//SOCKET_ERROR
    {
        qDebug()<<"send error";
        m_nFailedTimes++;
        logError(m_strIp+"|"+QString::number(m_nSmacqAddr)+"_data","1240发送数据错误");
        return;
    }

    if(m_qTcpSocket->waitForReadyRead(m_nTimeInterval) == false)
    {
        m_nFailedTimes++;
        logError(m_strIp+"|"+QString::number(m_nSmacqAddr)+"_data","1240发送数据错误");
        return ;
    }
    // 接收
    QByteArray cRcvBuf;
    cRcvBuf = m_qTcpSocket->readAll();
    int nRcvLen = cRcvBuf.size();
    QDateTime recvTime = QDateTime::currentDateTime();
    int nTime = sendTime.msecsTo(recvTime);
    if (m_bWriteDO)
    {
        Response_WriteDO(cRcvBuf, nRcvLen);
        m_bWriteDO = false;
    }
    else if (m_bStatus)
    {
        Response_ReadDO(cRcvBuf, nRcvLen);
        WriteCmdDO();
    }
    else
    {
        if(m_bSetSpeed)
        {
            m_bSetSpeed = false;
            Response_SetSpeedAI(cRcvBuf,nRcvLen);
        }
        else
        {
            Response_ReadAI(cRcvBuf, nRcvLen);
        }
    }
    qDebug()<<"time"<<nTime;
    emit sig_statisticsCounts(m_strIp+"|"+QString::number(m_nSmacqAddr),m_nSendTimes,m_nFailedTimes);
    emit sig_sendRecv(m_strIp+" type:思迈科华",cSendBuf,cRcvBuf);
    qDebug()<<m_strIp+"|"+QString::number(m_nSmacqAddr)+"_data";
    logInfo(m_strIp+"_"+QString::number(m_nSmacqAddr)+"_data","type:思迈科华 send:"+(QString)cSendBuf.toHex()+" recv:"+(QString)cRcvBuf.toHex());
}

ushort ScamqAIDO::CalCRC(QByteArray &pBuf, int nBufLen)
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

bool ScamqAIDO::Query_ReadAI(QByteArray &pBuf, int &nLen)
{
    int i = 0;

    pBuf[i++] = 0x01;//设备地址变量
    pBuf[i++] = 0x04;
    pBuf[i++] = 0x75;
    pBuf[i++] = (uchar)0x95;
    pBuf[i++] = 0x00;
    pBuf[i++] = m_nBitCount;	// 8个AI
    nLen = i;


    return true;
}

void ScamqAIDO::Response_ReadAI(QByteArray &pBuf, const int &nLen)
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

    for (int i = 0; i < 4; i++)
    {
        uchar ucB1  = (uchar)pBuf[i * 2 + 2 + 1];
        uchar ucB2  = (uchar)pBuf[i * 2 + 2 + 2];
        wVal = 	(ucB1 << 8) | ucB2;
    }
}

bool ScamqAIDO::Queray_WriteDO(QByteArray &pBuf, int &nLen)
{
    ushort wDO	= m_ctrlDO.wCmd;
    int  nReg	= m_ctrlDO.nRegAddr;
    int  nCount = m_ctrlDO.nRegCount;

    char cHigh = 0x00;
    char cLow  = 0x00;
    cHigh = wDO >> 8;
    //	cHigh = 0x00;
    cLow  = wDO & 0x00FF;	// 字节数据 -- 与数据组（m_wID)无关,值已经在WriteDO()转换

    int nFrameByteCount = 8;
    int nDataByteCount	= (nCount + 7) / 8;
    if (nCount > 0x08 )		// 最大支持16位操作
    {
        nFrameByteCount = 8 + 1;
    }

    //
    //
    //<S> 04  15  00  00  00 10  02  00 00
    //<R> 04  15  00  00  00 02
    //
    int i = 0;
    // 1.
    pBuf[i++] = m_nSmacqAddr;  //device address,应该是变量 0x04
    pBuf[i++] = 0x0F;	// Byte 0: FC = 0F (hex)

    pBuf[i++] = 0x00;	// Byte 1-2: Reference number
    pBuf[i++] = nReg;	// 8bit 一组

    pBuf[i++] = 0x00;	// Byte 3-4: Bit count (1-800)
    pBuf[i++] = nCount;

    pBuf[i++] = nDataByteCount;	// Byte 5: Byte count (B = (bit count + 7)/8)

    pBuf[i++] = cLow;	// 字节数据 - 位写数据

    if (nDataByteCount == 2)
        pBuf[i++] = cHigh;


    nLen = i;

    return true;
}

bool ScamqAIDO::Query_ReadDO(QByteArray &pBuf, int &nLen)
{
    int i = 0;
    pBuf[i++] = m_nSmacqAddr;  //应该是变量，代表设备地址，
    pBuf[i++] = 0x01;

    pBuf[i++] = 0x00;	// 地址
    pBuf[i++] = 0x01;	//  16bit 一组

    pBuf[i++] = 0x00;	// 数量
    pBuf[i++] = 0x10;

    nLen = i;

    qDebug()<<"readDo"<<pBuf;
    return true;
}

void ScamqAIDO::Response_WriteDO(QByteArray &pBuf, const int &nLen)
{
    int  nCount = m_ctrlDO.nRegCount;

    // 1. 接收长度校验
    if ( nLen != 8)
    {
        qDebug()<<"writeDO数据长度功能响应错误11111111";
        m_nFailedTimes++;

        return; // 接收不正确
    }

    // 2. 功能码+数据长度校验
    if (pBuf[0].operator !=(m_nSmacqAddr)     //应该是变量 设备地址
            || pBuf[1].operator !=(0x0F)
            || pBuf[2].operator !=(0x00)
            //	  || *(pBuf +  3) != 0x00
            || pBuf[4].operator !=(0x00)
            || pBuf[5].operator !=(nCount)
            )
    { // 接收不正确
        m_nFailedTimes++;
        qDebug()<<"writeDO数据长度功能响应错误";
        return;
    }

    // 接收正确 - 避免命令与通讯重叠
    if (m_ctrlDO.wState = wsReady)
        m_ctrlDO.wState = wsEnd;
}

void ScamqAIDO::Response_ReadDO(QByteArray &pBuf, const int &nLen)
{
    // 1. 接收长度校验
    if ( nLen != 7)
    {
        m_nFailedTimes++;
        qDebug()<<"readDO数据长度功能响应错误111111111"<<pBuf.size()<<pBuf;
        return; // 接收不正确
    }

    // 2. 功能码+数据长度校验
    if ( pBuf[0].operator !=(m_nSmacqAddr)  //设备地址变量
         ||pBuf[1].operator !=( 0x01)
         ||pBuf[2].operator !=( 0x02)
         )
    { // 接收不正确
        qDebug()<<"readDO数据长度功能响应错误";
        m_nFailedTimes++;
        return;
    }
    //

    uchar cLow = pBuf[3];
    uchar cHigh = pBuf[4];

    m_wDOState = cLow + (cHigh << 8);
}

bool ScamqAIDO::Query_SetSpeedAI(QByteArray &pBuf, int &nLen)
{
    int i = 0;

    pBuf[i++] = 0x01;//设备地址变量
    pBuf[i++] = 0x06;
    pBuf[i++] = 0x9c;
    pBuf[i++] = 0xb9;
    pBuf[i++] = 0x00;
    pBuf[i++] = 0x03;
    nLen = i;
    return true;
}

void ScamqAIDO::Response_SetSpeedAI(QByteArray &pBuf, const int &nLen)
{
    qDebug()<<"设置ai速度"<<pBuf<<nLen;
}

void ScamqAIDO::WriteCmdDO()
{
    if(m_bMarkState)
    {
        m_ctrlDO.wCmd = 0;
        m_bMarkState = false;
    }
    else
    {
        for(int i = 0; i < m_ctrlDO.nRegCount; i++)
        {
            m_ctrlDO.wCmd |= 1 << i;
        }
        m_bMarkState = true;
    }
    m_ctrlDO.wState = wsIni;
}

void ScamqAIDO::slt_readyRead()
{

}

void ScamqAIDO::setBitCount(int nCount)
{
    m_ctrlDO.nRegAddr = 1;
    m_ctrlDO.nRegCount = nCount;
    m_ctrlDO.wCmd = 0;
}

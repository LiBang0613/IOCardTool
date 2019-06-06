#include "e1211.h"

E1211::E1211()
{
    m_bMarkState = false;
    m_bWriteDO = false;
}

E1211::~E1211()
{

}

void E1211::setBitCount(int nCount)
{
    m_ctrlDO.nRegAddr = 0;
    m_ctrlDO.nRegCount = nCount;
    m_ctrlDO.wCmd = 0;
}

bool E1211::Query_Read(QByteArray &sendBuf, int &nLen)
{
    int i = 0;
    // 1.
    for (i = 0; i < 5; i++)
        sendBuf[i] = 0x00;
    // 2.
    sendBuf[i++]  = 0x06;
    sendBuf[i++] = 0x01;
    sendBuf[i++] = 0x01;

    sendBuf[i++] = 0x00;	// 地址
    sendBuf[i++] = 0x00;	//  16bit 一组

    sendBuf[i++] = 0x00;	// 数量
    sendBuf[i++] = 0x10;

    nLen = i;

    return true;
}

bool E1211::Response_Read(QByteArray recvBuf, int nLen)
{
    // 1. 接收长度校验
    if ( nLen != 0x05 + 0x05 + 0x01)
    {
        m_nFailedTimes++;
        logError(m_strIp,"1211数据长度接收不正确Response_Read");
        return false; // 接收不正确
    }

    // 2. 功能码+数据长度校验
    if ( recvBuf[6].operator !=(0x01)
         || recvBuf[7].operator !=(0x01)
         || recvBuf[8].operator !=(0x02)
         )
    { // 接收不正确
        m_nFailedTimes++;
        logError(m_strIp,"1211功能码接收不正确Response_Read");
        return false;
    }

    uchar cLow = recvBuf[9];
    uchar cHigh = recvBuf[10];
    ushort state = 0;
    state = (cLow << 8) | cHigh;
    return true;
}

bool E1211::Query_Write(QByteArray &sendBuf, int &nLen)
{
    int  nCount = m_ctrlDO.nRegCount;

    ushort wDO = m_ctrlDO.wCmd;//命令值
    int nReg = m_ctrlDO.nRegAddr;
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

    //<S> 00  00  00  00  00  09  01  15  00  00  00 10  02  00 00
    //<R> 00  00  00  00  00  06  01  15  00  00  00 02

    int i = 0;
    // 1.
    for (i = 0; i < 5; i++)
        sendBuf[i] = 0x00;
    // 2.
    sendBuf[i++] = nFrameByteCount;
    sendBuf[i++] = 0x01;
    sendBuf[i++] = 0x0F;	// Byte 0: FC = 0F (hex)

    sendBuf[i++] = 0x00;	// Byte 1-2: Reference number
    sendBuf[i++] = nReg;	// 8bit 一组

    sendBuf[i++] = 0x00;	// Byte 3-4: Bit count (1-800)
    sendBuf[i++] = nCount;

    sendBuf[i++] = nDataByteCount;	// Byte 5: Byte count (B = (bit count + 7)/8)

    sendBuf[i++] = cLow;	// 字节数据 - 位写数据

    if (nDataByteCount == 2)
        sendBuf[i++] = cHigh;

    nLen = i;
    return true;
}

bool E1211::Response_Write(QByteArray recvBuf, int nLen)
{
    // 1. 接收长度校验
    if ( nLen != 0x05 + 0x06 + 0x01)
    {
        m_nFailedTimes++;
        logError(m_strIp,"1211数据长度接收不正确Response_Write");
        return false; // 接收不正确
    }

    // 2. 功能码+数据长度校验
    if ( recvBuf[6].operator !=(0x01)
         || recvBuf[7].operator !=(0x0F)
         || recvBuf[8].operator !=(0x00)
         //	  || recvBuf[9].operator !=(0x00)
         || recvBuf[10].operator !=(0x00)
         || recvBuf[11].operator !=(m_ctrlDO.nRegCount)
         )
    { // 接收不正确
        m_nFailedTimes++;
        logError(m_strIp,"1211功能码接收不正确Response_Write");
        return false;
    }

    if(m_ctrlDO.wState == wsReady)
        m_ctrlDO.wState = wsEnd;

    return true;
}

void E1211::Process()
{
    QThread::msleep(m_nTimeInterval);
    QMutex mutex;
    mutex.lock();

    QByteArray sendArray;
    int nLen = 0;

    if(m_ctrlDO.wState != wsEnd)
    {
        m_bWriteDO = Query_Write(sendArray, nLen);
    }
    else
        Query_Read(sendArray, nLen);

    m_qTcpSocket->write(sendArray);
    m_nSendTimes++;

    m_sendArray = sendArray;

    if(m_qTcpSocket->waitForReadyRead() == false)
    {
        return ;
    }
    slt_readyRead();
    mutex.unlock();

}

void E1211::WriteCmdDO()
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

void E1211::slt_readyRead()
{
    //父类的接口，数采卡返回数据响应的槽函数
    QByteArray rcvArray = m_qTcpSocket->readAll();

    //    if(m_bWriteDO)
    if(rcvArray[7].operator ==(0x0f))
    {
        m_ctrlDO.wState = wsReady;
        Response_Write(rcvArray, rcvArray.size());
        m_bWriteDO = false;
    }
    else
    {
        Response_Read(rcvArray, rcvArray.size());

        WriteCmdDO();
    }
    emit sig_statisticsCounts(m_strIp,m_nSendTimes,m_nFailedTimes);
    emit sig_sendRecv(m_strIp+" type:1211",m_sendArray,rcvArray);
    logInfo(m_strIp,"type:1211 send:"+(QString)m_sendArray.toHex()+" recv:"+(QString)rcvArray.toHex());

}

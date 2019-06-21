#include "smacqdo.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif


SmacqDO::SmacqDO()
{
    m_wDOState = 0;
    m_unCount = 8;
    m_bMarkState = false;
    m_bWriteDO = false;
}

SmacqDO::~SmacqDO()
{

}

void SmacqDO::setBitCount(int nCount)
{
    m_ctrlDO.nRegAddr = 1;
    m_ctrlDO.nRegCount = nCount;
    m_ctrlDO.wCmd = 0;
}

void SmacqDO::Process()
{
    QThread::msleep(m_nTimeInterval);

    m_mutex->lock();
    QByteArray cSendBuf;
    int nSendLen = 0;
    int n = 0;

    // 检查是否有写位请求
    if(m_ctrlDO.wState != wsEnd)
    {
        m_bWriteDO = Queray_WriteDO(cSendBuf, nSendLen);
    }
    else
        Query_ReadDO(cSendBuf, nSendLen);

    ushort wCRC = CalCRC(cSendBuf, nSendLen);
    uchar cLowCRC = wCRC & 0xff;
    uchar cHighCRC = wCRC >> 8;
    cSendBuf[nSendLen++] = cLowCRC;
    cSendBuf[nSendLen++] = cHighCRC;

    qDebug()<<"crc"<<wCRC<<"sendBuf"<<cSendBuf;

    int nLen = m_qTcpSocket->write(cSendBuf);
    m_qTcpSocket->flush();
    m_nSendTimes++;
    m_sendArray = cSendBuf;
    if( nSendLen != nLen)	//SOCKET_ERROR
    {
        m_nFailedTimes++;
        qDebug()<<"send error";
        m_mutex->unlock();
        return ;
    }

    if(m_qTcpSocket->waitForReadyRead(1000) == false)
    {
        m_nFailedTimes++;
        qDebug()<<"readyread error";
        m_mutex->unlock();
        return;
    }
   slt_readyRead();
   m_mutex->unlock();
}

ushort SmacqDO::CalCRC(QByteArray& pBuf, int nBufLen)
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

bool SmacqDO::Queray_WriteDO(QByteArray &pBuf, int &nLen)
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

bool SmacqDO::Query_ReadDO(QByteArray &pBuf, int &nLen)
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

void SmacqDO::Response_WriteDO(QByteArray &pBuf, const int &nLen)
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

void SmacqDO::Response_ReadDO(QByteArray &pBuf, const int &nLen)
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


void SmacqDO::slt_readyRead()
{
    // 接收
    QByteArray recvBuf;
    recvBuf = m_qTcpSocket->readAll();
    int nRcvLen = recvBuf.size();
    if(recvBuf[1].operator ==(0x0f))
    {
        m_ctrlDO.wState = wsReady;
        Response_WriteDO(recvBuf, recvBuf.size());
        m_bWriteDO = false;
    }
    else
    {
        Response_ReadDO(recvBuf, recvBuf.size());
        WriteCmdDO();
    }

    emit sig_statisticsCounts(m_strIp+"|"+QString::number(m_nSmacqAddr),m_nSendTimes,m_nFailedTimes);
    emit sig_sendRecv(m_strIp+"|"+QString::number(m_nSmacqAddr)+" type:思迈科华DO",m_sendArray,recvBuf);
    logInfo(m_strIp+"|"+QString::number(m_nSmacqAddr)+"_data","type:思迈科华 send:"+(QString)m_sendArray.toHex()+" recv:"+(QString)recvBuf.toHex());
}

void SmacqDO::WriteCmdDO()
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

#include "e1240.h"

E1240::E1240()
{

}

E1240::~E1240()
{

}

bool E1240::Query_Read(QByteArray &sendBuf, int &nLen)
{
    int i = 0;

    // 1.
    for (i = 0; i < 5; i++)
        sendBuf[i] = 0x00;

    // 2.
    sendBuf[i++] = 0x06;
    sendBuf[i++] = 0x01;
    sendBuf[i++] = 0x04;
    sendBuf[i++] = 0x00;
    sendBuf[i++] = 0x00;
    sendBuf[i++] = 0x00;
    sendBuf[i++] = m_nBitCount;	// 8个AI

    nLen = i;
    return true;
}

bool E1240::Response_Read(QByteArray recvBuf, int nLen)
{
    // 1. 接收长度校验
    if ( nLen != 0x05 + 0x03 + 0x01 + m_nBitCount * 2)
        return false;

    // 2. 功能码+数据长度校验
    if ( recvBuf[6].operator !=(0x01)
         || recvBuf[7].operator !=(0x04)
         || recvBuf[8].operator !=(m_nBitCount * 2)
         )
        return false;	// 错误

    //	CSafeLock cLock(&m_cMutex);

    ushort wVal = 0;

    for (int i = 0; i < m_nBitCount; i++)
    {
        uchar ucB1  = recvBuf[i * 2 + 8 + 1];
        uchar ucB2  = recvBuf[i * 2 + 8 + 2];
        wVal = 	(ucB1 << 8) | ucB2;
    }
    return true;
}

void E1240::Process()
{
    QByteArray sendArray;
    int nSendLen = 0;
    Query_Read(sendArray,nSendLen);
    m_qTcpSocket.write(sendArray);
    if(nSendLen != sendArray.size())
    {
        qDebug()<<"send error";
        return;
    }
}

void E1240::slt_readyead()
{
    //父类的接口，数采卡返回数据响应的槽函数
    QByteArray recvArray = m_qTcpSocket.readAll();
    Response_Read(recvArray,recvArray.size());
}

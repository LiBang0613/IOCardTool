#include "e1211.h"

E1211::E1211()
{

}

E1211::~E1211()
{

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
            return false; // 接收不正确

        // 2. 功能码+数据长度校验
        if ( recvBuf[6].operator !=(0x01)
          || recvBuf[7].operator !=(0x01)
          || recvBuf[8].operator !=(0x02)
            )
        { // 接收不正确
            return false;
        }

        uchar cLow = recvBuf[9];
        uchar cHigh = recvBuf[10];
    return true;
}

bool E1211::Query_Write(QByteArray &sendBuf, int &nlen)
{
    return true;
}

bool E1211::Response_Write(QByteArray recvBuf, int nLen)
{
    // 1. 接收长度校验
    if ( nLen != 0x05 + 0x06 + 0x01)
        return false; // 接收不正确

    // 2. 功能码+数据长度校验
    if ( recvBuf[6].operator !=(0x01)
      || recvBuf[7].operator !=(0x0F)
      || recvBuf[8].operator !=(0x00)
//	  || recvBuf[9].operator !=(0x00)
      || recvBuf[10].operator !=(0x00)
      || recvBuf[11].operator !=(m_nBitCount)
        )
    { // 接收不正确
        return false;
    }
    return true;
}

void E1211::Process()
{

}

#include "e1211.h"

E1211::E1211()
{

}

E1211::~E1211()
{

}

bool E1211::Query_Read(QByteArray &sendBuf, int &nlen)
{
    return true;
}

bool E1211::Response_Read(QByteArray recvBuf, int nLen)
{
    return true;
}

bool E1211::Query_Write(QByteArray &sendBuf, int &nlen)
{
    return true;
}

bool E1211::Response_Write(QByteArray recvBuf, int nLen)
{
    return true;
}

void E1211::Process()
{

}

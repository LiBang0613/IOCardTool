#include "yd516p.h"

YD516P::YD516P()
{
    m_nCommand = 0;
}

YD516P::~YD516P()
{

}

ushort YD516P::getCheckCode()
{
    ushort code = 0;
    code = m_nCommand*256+82+m_nSmacqAddr;
    return code;
}

bool YD516P::Query_Read(QByteArray &sendBuf, int &nLen)
{
    int i = 0;


    sendBuf[i++] = (uchar)m_nSmacqAddr+0x80;
    sendBuf[i++] = (uchar)m_nSmacqAddr+0x80;
    sendBuf[i++] = 0x52;
    sendBuf[i++] = m_nCommand;
    sendBuf[i++] = 0x00;
    sendBuf[i++] = 0x00;
    ushort wCRC = getCheckCode();
    uchar cLowCRC = wCRC & 0xff;
    uchar cHighCRC = wCRC >> 8;
    sendBuf[i++] = cLowCRC;
    sendBuf[i++] = cHighCRC;

    nLen = i;
    return true;
}

bool YD516P::Response_Read(QByteArray &recvBuf, int nLen)
{
    return true;
}

void YD516P::Process()
{
    QThread::msleep(m_nTimeInterval);

    m_mutex->lock();
    QByteArray sendArray;
    int nSendLen = 0;
    Query_Read(sendArray,nSendLen);
    m_sendArray = sendArray;
    //    qDebug()<<"1240"<<QThread::currentThreadId()<<m_sendArray<<m_sendArray.size();
    m_qTcpSocket->write(sendArray);
    m_qTcpSocket->flush();
    m_nSendTimes++;
    if(nSendLen != sendArray.size())
    {
        qDebug()<<"send error";
        logError(m_strIp+"_data","516P发送数据错误");
        m_mutex->unlock();
        return;
    }

    if(m_qTcpSocket->waitForReadyRead(1000) == false)
    {
        m_mutex->unlock();
        return ;
    }

    slt_readyRead();
    m_mutex->unlock();
}

void YD516P::slt_readyRead()
{
    //父类的接口，数采卡返回数据响应的槽函数

    QByteArray recvArray = m_qTcpSocket->readAll();
    Response_Read(recvArray,recvArray.size());
    qDebug()<<"1240"<<QThread::currentThreadId()<<recvArray<<recvArray.size();
    emit sig_statisticsCounts(m_strIp+"|"+QString::number(m_nSmacqAddr),m_nSendTimes,m_nFailedTimes);
    emit sig_sendRecv(m_strIp+" type:516P",m_sendArray,recvArray);
    logInfo(m_strIp+"_data"," type:516P send:"+(QString)m_sendArray.toHex()+"recv:"+(QString)recvArray.toHex());
}

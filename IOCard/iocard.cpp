#include "iocard.h"


IOCard::IOCard()
{
    m_pThread = new QThread;
}

IOCard::~IOCard()
{
    if(m_pThread->isRunning())
    {
        m_pThread->wait();
        m_pThread->quit();
    }
}

bool IOCard::Open(QString strIp, uint nPort)
{
    connectCard(m_qTcpSocket,strIp,nPort);
    this->moveToThread(m_pThread);
    m_pThread->start();
    return true;
}

void IOCard::setBitCount(int nCount)
{
    m_nBitCount = nCount;
}

bool IOCard::connectCard(QTcpSocket &socket, QString strIp, uint nPort)
{
    connect(&socket,SIGNAL(connected()),this,SLOT(slt_tcpConnected()));
    connect(&socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(slt_recvSocketState(QAbstractSocket::SocketState)));
    connect(&socket,SIGNAL(readyRead()),this,SLOT(slt_readyRead()));
    socket.connectToHost(strIp,nPort);

    return true;
}

void IOCard::startThread()
{

}

void IOCard::stopThread()
{

}

void IOCard::slt_tcpConnected()
{

}

void IOCard::slt_recvSocketState(QAbstractSocket::SocketState)
{

}

void IOCard::slt_readyRead()
{

}

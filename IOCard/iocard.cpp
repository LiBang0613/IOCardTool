#include "iocard.h"


IOCard::IOCard()
{
    m_pThread = new QThread;
    connect(&m_qTcpSocket,SIGNAL(connected()),this,SLOT(slt_tcpConnected()));
    connect(&m_qTcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(slt_recvSocketState(QAbstractSocket::SocketState)));
    connect(&m_qTcpSocket,SIGNAL(readyRead()),this,SLOT(slt_readyRead()));
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
    this->moveToThread(m_pThread);
    m_pThread->start();
    connectCard(m_qTcpSocket,strIp,nPort);

    return true;
}

void IOCard::setBitCount(int nCount)
{
    m_nBitCount = nCount;
}

bool IOCard::connectCard(QTcpSocket &socket, QString strIp, uint nPort)
{
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
    qDebug()<<"connected success";
}

void IOCard::slt_recvSocketState(QAbstractSocket::SocketState state)
{
    qDebug()<<"state:"<<state;
}


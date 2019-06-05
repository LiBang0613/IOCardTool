#include "iocard.h"


IOCard::IOCard()
{
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    m_nBitCount = 0;
    m_nSendTimes = 0;
    m_nFailedTimes = 0;
    m_nTimeInterval = 1000;
    m_pThread = new QThread;
    connect(m_pThread,SIGNAL(finished()),m_pThread,SLOT(deleteLater()));
    connect(&m_qTcpSocket,SIGNAL(connected()),this,SLOT(slt_tcpConnected()));
    connect(&m_qTcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(slt_recvSocketState(QAbstractSocket::SocketState)));
    connect(&m_qTcpSocket,SIGNAL(readyRead()),this,SLOT(slt_readyRead()));
    connect(&m_qTcpSocket,SIGNAL(disconnected()),this,SLOT(slt_tcpDisConnected()));
    connect(&m_cycleTimer,SIGNAL(timeout()),this,SLOT(slt_ProThread()));
}

IOCard::~IOCard()
{
    if(m_pThread->isRunning())
    {
        m_pThread->quit();
        m_pThread->wait();
    }
}

bool IOCard::Open(QString strIp, uint nPort)
{
    this->moveToThread(m_pThread);
    m_pThread->start();
    m_strIp = strIp;
    if(m_qTcpSocket.state() == QTcpSocket::ConnectedState)
        m_qTcpSocket.disconnectFromHost();

    m_qTcpSocket.connectToHost(strIp,nPort);

    return true;
}

void IOCard::setBitCount(int nCount)
{
    m_nBitCount = nCount;
}

void IOCard::startThread()
{
    m_cycleTimer.start(m_nTimeInterval);
}

void IOCard::stopThread()
{
    if(m_pThread->isRunning())
    {
        m_pThread->quit();
        m_pThread->wait();
    }
    if(m_qTcpSocket.state() == QTcpSocket::ConnectedState)
        m_qTcpSocket.disconnectFromHost();

    m_cycleTimer.stop();
}

void IOCard::setTimeInterval(int second)
{
    m_nTimeInterval = second;
}

void IOCard::slt_tcpConnected()
{
    qDebug()<<"connected success";
}

void IOCard::slt_recvSocketState(QAbstractSocket::SocketState state)
{
    qDebug()<<"state:"<<state;
}

void IOCard::slt_ProThread()
{
    Process();
}

void IOCard::slt_tcpDisConnected()
{
    qDebug()<<"disconnected";
}


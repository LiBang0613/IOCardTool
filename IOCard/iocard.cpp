#include "iocard.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

IOCard::IOCard()
{
    qDebug()<<"main"<<QThread::currentThreadId();
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    m_nBitCount = 0;
    m_nSendTimes = 0;
    m_nFailedTimes = 0;
    m_nTimeInterval = 1000;
    m_bNewSocket = false;
    m_pThread = new QThread;

    m_bExitThread = false;

    //    connect(m_pThread,SIGNAL(finished()),m_pThread,SLOT(deleteLater()));
    connect(this,SIGNAL(sig_newSocket()),this,SLOT(slt_newSocket()),Qt::QueuedConnection);
    connect(this,SIGNAL(sig_stopThread()),this,SLOT(slt_stopThread()),Qt::QueuedConnection);

    connect(this, SIGNAL(sig_operate()), this, SLOT(slt_ProThread()), Qt::QueuedConnection);
    connect(this, SIGNAL(sig_reConnect(QString,uint)), this, SLOT(slt_reConnect(QString,uint)), Qt::QueuedConnection);
}

IOCard::~IOCard()
{
    if(m_pThread->isRunning())
    {
        m_pThread->quit();
        m_pThread->wait();
    }
    delete m_pThread;
    delete m_qTcpSocket;
}

bool IOCard::Open(QString strIp, uint nPort)
{
    this->moveToThread(m_pThread);
    if(!m_pThread->isRunning())
        m_pThread->start();
    m_strIp = strIp;
    if(!m_bNewSocket)
    {
        emit sig_newSocket();
    }
    else
    {
        if(m_qTcpSocket->state() == QTcpSocket::ConnectedState)
            m_qTcpSocket->disconnectFromHost();

        m_qTcpSocket->connectToHost(strIp,nPort);
    }


    return true;
}

void IOCard::reConnect(QString strIP, uint nPort)
{
    emit sig_reConnect(strIP, nPort);
}

void IOCard::setBitCount(int nCount)
{
    m_nBitCount = nCount;
}

void IOCard::startThread()
{
    m_bExitThread = false;
    emit sig_operate();
}

void IOCard::stopThread()
{
    m_bExitThread = true;
}

void IOCard::setTimeInterval(int second)
{
    m_nTimeInterval = second;
}

void IOCard::closeThread()
{
    m_bExitThread = true;
    emit sig_stopThread();
    if(m_pThread->isRunning())
    {
        m_pThread->quit();
        m_pThread->wait();
    }
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
    qDebug()<<"slt_proThread"<<QThread::currentThreadId()<<m_qTcpSocket->state();
    while(!m_bExitThread)
    {
        if(m_qTcpSocket->state() == QTcpSocket::ConnectedState)
        {
            Process();
        }
        else
        {
            stopThread();
            emit sig_connectFailed(m_strIp);
        }
    }
}

void IOCard::slt_newSocket()
{
    m_qTcpSocket = new QTcpSocket;
    //    connect(m_qTcpSocket,SIGNAL(connected()),this,SLOT(slt_tcpConnected()),Qt::QueuedConnection);
    //    connect(m_qTcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(slt_recvSocketState(QAbstractSocket::SocketState)),Qt::QueuedConnection);
    //    connect(m_qTcpSocket,SIGNAL(readyRead()),this,SLOT(slt_readyRead()),Qt::QueuedConnection);
    connect(m_qTcpSocket,SIGNAL(disconnected()),this,SLOT(slt_tcpDisConnected()),Qt::QueuedConnection);

    m_qTcpSocket->connectToHost(m_strIp,502);
    m_qTcpSocket->setReadBufferSize(1024);
    if(!m_qTcpSocket->waitForConnected(5000))
    {
        logInfo(m_strIp,"ip地址连接失败");
        stopThread();
        emit sig_connectFailed(m_strIp);
        return;
    }
    qDebug()<<m_qTcpSocket->state();
    m_bNewSocket = true;

}

void IOCard::slt_stopThread()
{
    if(m_qTcpSocket->state() == QTcpSocket::ConnectedState)
        m_qTcpSocket->disconnectFromHost();
    m_qTcpSocket->waitForDisconnected();
}

void IOCard::slt_tcpDisConnected()
{
    qDebug()<<"disconnected";
}

void IOCard::slt_reConnect(QString strIP, uint nPort)
{
    if(m_qTcpSocket != NULL && m_qTcpSocket->state() != QTcpSocket::ConnectedState)
    {
        m_qTcpSocket->connectToHost(strIP, nPort);
        if(m_qTcpSocket->waitForConnected(5000) == false)
        {
            emit sig_connectFailed(strIP);
            return;
        }
    }
}

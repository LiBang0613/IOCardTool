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
    m_nPort = 0;
    m_pThread = new QThread;

    m_bExitThread = false;

    //    connect(m_pThread,SIGNAL(finished()),m_pThread,SLOT(deleteLater()));
    connect(this,SIGNAL(sig_newSocket()),this,SLOT(slt_newSocket())/*,Qt::QueuedConnection*/);
    connect(this,SIGNAL(sig_stopThread()),this,SLOT(slt_stopThread())/*,Qt::QueuedConnection*/);

    connect(this, SIGNAL(sig_operate()), this, SLOT(slt_ProThread())/*, Qt::QueuedConnection*/);
    connect(this, SIGNAL(sig_reConnect(QString,uint)), this, SLOT(slt_reConnect(QString,uint))/*, Qt::QueuedConnection*/);
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
    m_nPort = nPort;
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
    emit sig_reConnect(strIP, m_nPort);
}

void IOCard::setBitCount(int nCount)
{
    m_nBitCount = nCount;
}

void IOCard::startThread()
{
    m_bExitThread = false;
    emit sig_operate();
    logInfo(m_strIp+"_net","开始线程");
}

void IOCard::stopThread()
{
    m_bExitThread = true;
    logInfo(m_strIp+"_net","停止线程");
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
    logInfo(m_strIp+"_net","关闭线程");
}

void IOCard::setDeviceAddress(int addr)
{
    m_nSmacqAddr  = addr;
}

void IOCard::setMutex(QMutex *mutex)
{

    m_mutex = mutex;
    qDebug()<<m_mutex;
}

void IOCard::slt_ProThread()
{
    qDebug()<<"slt_proThread"<<QThread::currentThreadId()<<m_qTcpSocket->state();

    while(!m_bExitThread)
    {
//        m_mutex->lock();
        if(m_qTcpSocket->state() == QTcpSocket::ConnectedState)
        {
            Process();
        }
        else
        {
            stopThread();
            emit sig_connectFailed(m_strIp);
        }
//        m_mutex->unlock();
    }

}

void IOCard::slt_newSocket()
{
    m_qTcpSocket = new QTcpSocket;
    connect(m_qTcpSocket,SIGNAL(disconnected()),this,SLOT(slt_tcpDisConnected()),Qt::QueuedConnection);

    m_qTcpSocket->connectToHost(m_strIp,m_nPort);
    m_qTcpSocket->setReadBufferSize(1024);
    if(!m_qTcpSocket->waitForConnected(5000))
    {
        logInfo(m_strIp+"_net","ip地址连接失败");
        qDebug()<<m_qTcpSocket->errorString();
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
    logInfo(m_strIp+"_net","socket断开连接");
}

void IOCard::slt_tcpDisConnected()
{
    qDebug()<<"disconnected"<<m_qTcpSocket->errorString();
    logInfo(m_strIp+"_net","socket主动断开连接，disconnect");
}

void IOCard::slt_reConnect(QString strIP, uint nPort)
{
    if(m_qTcpSocket != NULL && m_qTcpSocket->state() != QTcpSocket::ConnectedState)
    {
        m_qTcpSocket->connectToHost(strIP, nPort);
        if(m_qTcpSocket->waitForConnected(5000) == false)
        {
            emit sig_connectFailed(strIP);
            logInfo(m_strIp+"_net","socket重新连接失败");
            return;
        }
    }
}

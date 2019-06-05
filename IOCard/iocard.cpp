﻿#include "iocard.h"


IOCard::IOCard()
{
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    m_nBitCount = 0;
    m_nSendTimes = 0;
    m_nFailedTimes = 0;
    m_nTimeInterval = 1000;
    m_bNewSocket = false;
    m_pThread = new QThread;
    connect(m_pThread,SIGNAL(finished()),m_pThread,SLOT(deleteLater()));
    connect(&m_cycleTimer,SIGNAL(timeout()),this,SLOT(slt_ProThread()),Qt::QueuedConnection);
    connect(this,SIGNAL(sig_newSocket()),this,SLOT(slt_newSocket()));
    connect(this,SIGNAL(sig_stopThread()),this,SLOT(slt_stopThread()));
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
    emit sig_stopThread();
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
    qDebug()<<"id"<<QThread::currentThreadId();
    Process();
}

void IOCard::slt_newSocket()
{
    m_qTcpSocket = new QTcpSocket(this);
    connect(m_qTcpSocket,SIGNAL(connected()),this,SLOT(slt_tcpConnected()),Qt::QueuedConnection);
    connect(m_qTcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(slt_recvSocketState(QAbstractSocket::SocketState)),Qt::QueuedConnection);
    connect(m_qTcpSocket,SIGNAL(readyRead()),this,SLOT(slt_readyRead()),Qt::QueuedConnection);
    connect(m_qTcpSocket,SIGNAL(disconnected()),this,SLOT(slt_tcpDisConnected()),Qt::QueuedConnection);

    m_qTcpSocket->connectToHost(m_strIp,502);
    m_bNewSocket = true;

}

void IOCard::slt_stopThread()
{
    if(m_qTcpSocket->state() == QTcpSocket::ConnectedState)
        m_qTcpSocket->disconnectFromHost();

    if(m_pThread->isRunning())
    {
        m_pThread->quit();
        m_pThread->wait();
    }
}

void IOCard::slt_tcpDisConnected()
{
    qDebug()<<"disconnected";
}


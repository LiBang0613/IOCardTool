#include "logexport.h"
#include <QCoreApplication>
#include <QDebug>
#include <QThread>

LogExport *ManageLog::m_logExport = nullptr;
QThread *ManageLog::m_exportThread = nullptr;
ManageLog *ManageLog::m_manageLog = nullptr;
int ManageLog::m_iLogLevel = 0;

LogExport::LogExport()
{
    m_iSaveDays     = -1;
    m_iFileMaxSize = 1024*1024*7;
    m_strLocalPath    = qApp->applicationDirPath()+"/Log/";
    m_nFileCount = 0;
    QDir dir(m_strLocalPath);
    if(!dir.exists())
    {
        dir.mkdir(dir.absolutePath());
    }

    m_strCurrentLogDate  = QDateTime::currentDateTime().toString("yyyyMMdd");
}

LogExport::~LogExport()
{
    QByteArray array(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]").toLatin1());
    array.append("<info> 程序关闭！");
    slt_receiveLog(array,LogType::Chart);
}

void LogExport::setFileMaxSize(int fileMaxSize)
{
    m_iFileMaxSize = fileMaxSize;
}

void LogExport::setLogSaveDays(int nDays)
{
    m_iSaveDays = nDays;
}

void LogExport::slt_receiveLog(const QByteArray &info, const int &type)
{
    m_logMutex.lock();
    updateCurrentFileName((LogType)type);

    QFile file(m_strCurrentFileName);
//    qDebug()<<m_strCurrentFileName<<m_iFileMaxSize<<file.size()<<"---------";
    if(file.open(QFile::WriteOnly | QFile::Append | QFile::Text))
    {
        file.write(info);
        file.write("\n");
    }
    file.flush();
    file.close();
    m_logMutex.unlock();
}

void LogExport::slt_receiveLog(const QByteArray & info, const QString &name)
{
    m_logMutex.lock();
    updateCurrentFileName(name);

    QFile file(m_strCurrentFileName);
//    qDebug()<<m_strCurrentFileName<<m_iFileMaxSize<<file.size()<<"---------";
    if(file.open(QFile::WriteOnly | QFile::Append | QFile::Text))
    {
        file.write(info);
        file.write("\n");
    }
    file.flush();
    file.close();
    m_logMutex.unlock();
}

void LogExport::slt_deleteTimeOut()
{
    if(m_iSaveDays>0)
    {
        QDateTime currentTime = QDateTime::currentDateTime();
        QDir dir(m_strLocalPath+"/Log");

        QFileInfoList fileInfoList = dir.entryInfoList();
        foreach(QFileInfo fileInfo, fileInfoList)
        {
            if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
                continue;
            QString fileName = fileInfo.fileName();
            if(fileName.size()>8)
            {
                QDateTime creatTime = QDateTime::fromString(fileName.left(8),"yyyyMMdd");
                if( currentTime.daysTo(creatTime)>=m_iSaveDays)
                {
                    dir.remove(fileInfo.fileName());
                }
            }
        }
    }
}

void LogExport::updateCurrentFileName(LogType type)
{
    QString suffix;
    switch (type)
    {
    case Net:
        suffix = "_Net";
        break;
    case Chart:
        suffix = "_Chart";
        break;
    }

    if(!m_mapLogFileCount.contains(type))
    {
        m_mapLogFileCount.insert(type,0);
    }
    if(m_strCurrentLogDate != QDateTime::currentDateTime().toString("yyyyMMdd"))
    {
        m_strCurrentLogDate = QDateTime::currentDateTime().toString("yyyyMMdd");
        m_mapLogFileCount[type] = 0;
    }
    int count = m_mapLogFileCount.value(type);
    m_strCurrentFileName = m_strLocalPath + m_strCurrentLogDate+"_"+
            QString("%1").arg(count,3,10,QLatin1Char('0'))+suffix+".log";

    QFile file(m_strCurrentFileName);
    if(file.size() >= m_iFileMaxSize)
    {
        int count = m_mapLogFileCount.value(type);
        count++;
        m_mapLogFileCount.insert(type,count);
        m_strCurrentFileName = m_strLocalPath + m_strCurrentLogDate+"_"+
                QString("%1").arg(count,3,10,QLatin1Char('0'))+suffix+".log";
    }
}

void LogExport::updateCurrentFileName(QString name)
{
    QString suffix = name;

    if(m_strCurrentLogDate != QDateTime::currentDateTime().toString("yyyyMMdd"))
    {
        m_strCurrentLogDate = QDateTime::currentDateTime().toString("yyyyMMdd");
        m_nFileCount = 0;
    }

    m_strCurrentFileName = m_strLocalPath + m_strCurrentLogDate+"_"+
            QString("%1").arg(m_nFileCount,3,10,QLatin1Char('0'))+"_"+suffix+".log";

    QFile file(m_strCurrentFileName);
    if(file.size() >= m_iFileMaxSize)
    {
        m_nFileCount++;
        m_strCurrentFileName = m_strLocalPath + m_strCurrentLogDate+"_"+
                QString("%1").arg(m_nFileCount,3,10,QLatin1Char('0'))+"_"+suffix+".log";
    }
}


ManageLog::ManageLog()
{
    if(m_logExport!=NULL)
    {
        m_deleteLogTimer = new QTimer(this);
        connect(m_deleteLogTimer,SIGNAL(timeout()),m_logExport,SLOT(slt_deleteTimeOut()),Qt::QueuedConnection);
        m_deleteLogTimer->start(1000);
        m_iLogLevel = LogLevel::info;
    }
}

ManageLog::~ManageLog()
{
}

ManageLog* ManageLog::getLogExport()
{
    QMutex mutex;
    mutex.lock();

    if(m_logExport == nullptr)
    {
        m_logExport = new LogExport;
    }

    if(m_manageLog == nullptr)
    {
        m_manageLog = new ManageLog;
        connect(m_manageLog,SIGNAL(sig_receiveLog(QByteArray,int)),m_logExport,SLOT(slt_receiveLog(QByteArray,int)),Qt::QueuedConnection);
        connect(m_manageLog,SIGNAL(sig_receiveLog(QByteArray,QString)),m_logExport,SLOT(slt_receiveLog(QByteArray,QString)),Qt::QueuedConnection);
    }

    startExportThread();

    mutex.unlock();
    return m_manageLog;
}

void ManageLog::startExportThread()
{
    QMutex mutex;
    mutex.lock();
    if(m_exportThread == nullptr)
    {
        m_exportThread = new QThread;

        connect( m_exportThread, SIGNAL(finished()), m_exportThread, SLOT(deleteLater()) );

        if(m_logExport!=NULL)
        {
            connect( m_exportThread, SIGNAL(finished()), m_logExport, SLOT(deleteLater()) );
            m_logExport->moveToThread(m_exportThread);
            m_exportThread->start();
        }
    }
    mutex.unlock();
}

bool ManageLog::printLog(LogLevel level, LogType type, QString log)
{
    if(m_logExport == nullptr)
    {
        return false;
    }
    if(level < m_iLogLevel)
    {
        return false;
    }
    QByteArray array;
    array.append(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss:zzz]"));
    switch (level)
    {
    case debug:
    {
        array.append("<debug> ");
        break;
    }
    case info:
        array.append("<info> ");
        break;
    case warn:
        array.append("<warning> ");
        break;
    case err:
        array.append("<error> ");
        break;
    case critical:
        array.append("<critical> ");
        break;
    }
    array.append(log);
    if(m_manageLog!=NULL)
    {
        m_manageLog->sendReceiveLogSig(array,type);
    }
    return true;
}

bool ManageLog::printLog(LogLevel level, QString name, QString log)
{
    if(m_logExport == nullptr)
    {
        return false;
    }
    if(level < m_iLogLevel)
    {
        return false;
    }
    QByteArray array;
    array.append(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss:zzz]"));
    switch (level)
    {
    case debug:
    {
        array.append("<debug> ");
        break;
    }
    case info:
        array.append("<info> ");
        break;
    case warn:
        array.append("<warning> ");
        break;
    case err:
        array.append("<error> ");
        break;
    case critical:
        array.append("<critical> ");
        break;
    }
    array.append(log);
    if(m_manageLog!=NULL)
    {
        m_manageLog->sendReceiveLogSig(array,name);
    }
    return true;
}

void ManageLog::deleteManageLog()
{
    if(m_exportThread!=NULL)
    {
        m_exportThread->quit();
        m_exportThread->wait();
    }

    if(m_manageLog!=NULL)
    {
        m_manageLog->deleteLater();
    }
}

void ManageLog::setLogLevel(LogLevel level)
{
    m_iLogLevel = level;
}

void ManageLog::setLogFileSize(const int& fileSize)
{
    if(m_logExport != nullptr)
    {
        m_logExport->setFileMaxSize(fileSize);
    }
}

void ManageLog::setLogSaveDays(const int& nDays)
{
    if(m_logExport != nullptr)
    {
        m_logExport->setLogSaveDays(nDays);
    }
}
void ManageLog::sendReceiveLogSig(const QByteArray &info, const int &type)
{
    emit sig_receiveLog(info,type);
}

void ManageLog::sendReceiveLogSig(const QByteArray &info, const QString &name)
{
    emit sig_receiveLog(info,name);
}


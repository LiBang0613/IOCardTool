#ifndef LOGEXPORT_H
#define LOGEXPORT_H
#include <QObject>
#include <QThread>
#include <QFile>
#include <QTime>
#include <QDir>
#include <QMutex>
#include <QMetaType>
#include <QTimer>
#include <QSet>
#include <QMap>
#include "iocard_global.h"

enum LogLevel
{
    debug = 0,
    info = 1,
    warn = 2,
    err = 3,
    critical = 4
};
enum LogType
{
    Net = 0,
    Chart = 1
};

class IOCARDSHARED_EXPORT LogExport:public QObject
{
    Q_OBJECT
public:
    LogExport();//大于nDay天的日志文件将会被删除，-1代表程序不会自动删除日志文件
    ~LogExport();

    //日志输出类设置文件最大小值的接口
    void setFileMaxSize(int fileMaxSize);
    //日志输出类设置日志文件保存最大天数的接口
    void setLogSaveDays(int nDays);

private slots:
    //接收从ManageLog管理类发送的日志文件类型和日志内容的槽函数，并将日志内容写入文件中。
    void slt_receiveLog(const QByteArray &, const int &);
    void slt_receiveLog(const QByteArray &, const QString  &);
    //ManageLog管理类删除文件定时器的槽函数。
    void slt_deleteTimeOut();

private:
    //更新当前文件的路径，根据不同日志文件类型。
    void updateCurrentFileName(LogType type);
    void updateCurrentFileName(QString name);

private:
    QMutex  m_logMutex;//加锁避免多线程同时写文件异常
    QString m_strLocalPath;//可执行文件夹下Log文件夹的路径
    QString m_strCurrentFileName;//当前写入文件的文件名
    QString m_strCurrentLogDate;//当前日期
    int m_iFileMaxSize;//每个日志文件的最大大小
    int m_iSaveDays;    //日志文件保存的时常，单位是天
    QMap<LogType,int> m_mapLogFileCount;//保存当前不同类型日志的点击文件计数
    QMap<QString,int> m_mapFileCount;

};


class IOCARDSHARED_EXPORT ManageLog:public QObject
{
    Q_OBJECT
public:
    //获取日志管理类的接口，每次获得的都是同一个对象，通过这个接口获取日志管理类。
    static ManageLog *getLogExport();
    //打印日志的接口
    static bool printLog(LogLevel level,LogType type, QString log);
    static bool printLog(LogLevel level,QString name, QString log);
    //外部调用释放资源的接口
    static void deleteManageLog();

    //设置日志输出的等级，等级以下的日志不会写入到文件的接口
    void setLogLevel(LogLevel level);
    //设置日志文件的文件写入大小，通过这个接口，在调用LogExport设置日志文件大小的接口。
    void setLogFileSize(const int &fileSize);
    //设置日志文件的文件保存天数，通过这个接口，在调用LogExport设置日志保存天数的接口。
    void setLogSaveDays(const int &nDays);

private:
    //禁止调用构造函数和拷贝函数和赋值函数
    ManageLog();
    virtual ~ManageLog();
    ManageLog(const ManageLog&) {}
    ManageLog &operator =(const ManageLog&) {return *this;}
signals:
    //发送日志信息和日志类型的信号。
    void sig_receiveLog(QByteArray,int);
    void sig_receiveLog(QByteArray,QString);

private:
    //实例化LogExport类的线程，并通过信号槽方式调用线程完毕自动删除线程的连接。
    static void startExportThread();
    //发送日志等级信息给LogExport，通过信号槽方式发送
    void sendReceiveLogSig(const QByteArray &info,const int &type);
    void sendReceiveLogSig(const QByteArray &info,const QString &name);

private:
    static LogExport *m_logExport;//日志输出类的对象
    static QThread *m_exportThread;//LogExport的线程
    static ManageLog *m_manageLog;//管理LogExport日志输出类的对象
    static int m_iLogLevel;//初始化日志等级，该等级以下得日志不会输出到日志文件中。

private:
    QTimer *m_deleteLogTimer;//删除逾期日志文件得定时器
};

//调用ManageLog类的printLog函数定义成不同等级得宏，输出日志直接调用宏。
#define logDebug(x,y)  ManageLog::printLog(LogLevel::debug,x,y)
#define logInfo(x,y)  ManageLog::printLog(LogLevel::info,x,y)
#define logWarning(x,y)  ManageLog::printLog(LogLevel::warn,x,y)
#define logError(x,y)  ManageLog::printLog(LogLevel::err,x,y)
#define logCritical(x,y)  ManageLog::printLog(LogLevel::critical,x,y)
#endif // LOGEXPORT_H

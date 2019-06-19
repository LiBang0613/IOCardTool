#ifndef MULTIPLESET_H
#define MULTIPLESET_H

#include <QWidget>
#include <QMessageBox>
#include "daqset.h"
#include "device.h"
enum DeviceType
{
    dtDevice1 = 0,			//仪器1根据需求暂定1个1211和1个1240
    dtDevice2 = 1,			//仪器2根据需求暂定3个1211
    dtDevice3 = 2,			//仪器3根据需求暂定3个1240
    dtDevice4 = 3,			//仪器4根据需求暂定2个1240和1个1211
    dtDevice5 = 4,
};

namespace Ui {
class MultipleSet;
}

class MultipleSet : public QWidget
{
    Q_OBJECT

public:
    explicit MultipleSet(QWidget *parent = 0);
    ~MultipleSet();

private slots:
    //保存按钮的槽函数，点击保存，保存配置信息并展示在表格中。
    void on_pb_saveInfo_clicked();

    //开始按钮的槽函数，选中设备点击开始进行测试
    void on_pb_start_clicked();

    //暂停按钮的槽函数，暂停选中设备的测试
    void on_pb_stop_clicked();

    //定时器清空文本框内的文本
    void slt_clearTextEdit();

    //接收设备对象的发送和接收到的数据，显示在文本区域中。
    void slt_recvDeviceInfo(const QString &ip,const QByteArray &before,const QByteArray &after);

    //接收设备对象的发送命令的总次数和失败次数的槽函数
    void slt_receDeviceTimes(const QString &Ip, const int &total, const int &failed);

    //接收设备对象发送的连接失败的信号，并根据提示是否进行重新连接。
    void slt_recvConnectFailed(QString ip);

    //删除按钮的槽函数，删除保存的设备的配置信息和设备对象的指针；
    void on_pb_deleteSet_clicked();


private:
    //判断输入的配置信息是否正确
    bool judgeSettingInfo();
    //获取拼接四个文本框中的ip地址
    QString getIpAddr();
private:
    Ui::MultipleSet *ui;

    QMap<QString,RunTime> m_mapDeviceRunTime;       //保存设备的开始时间和运行状态
    QMap<QString,IODevice*> m_mapDeviceObject;      //保存生成的IODevice设备的对象的指针
    QTimer m_clearTextTimer;            //定时清理文本框中文本的定时器。

};

#endif // MULTIPLESET_H

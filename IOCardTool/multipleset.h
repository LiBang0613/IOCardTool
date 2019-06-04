#ifndef MULTIPLESET_H
#define MULTIPLESET_H

#include <QWidget>
#include <QMessageBox>
enum DeviceType
{
    dtDevice1 = 0,			//仪器1根据需求暂定2个1211和1个1240
    dtDevice2 = 1,			//仪器2根据需求暂定3个1211
    dtDevice3 = 2,			//仪器3根据需求暂定3个1240
    dtDevice4 = 3,			//仪器4根据需求暂定2个1240和1个1211
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
    void on_pb_saveInfo_clicked();

    void on_pb_start_clicked();

    void on_pb_stop_clicked();

    void slt_clearTextEdit();

private:
    bool judgeSettingInfo();
    QString getIpAddr();
private:
    Ui::MultipleSet *ui;

};

#endif // MULTIPLESET_H

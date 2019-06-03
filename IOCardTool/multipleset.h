#ifndef MULTIPLESET_H
#define MULTIPLESET_H

#include <QWidget>
#include <QMessageBox>

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

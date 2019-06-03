#include "multipleset.h"
#include "ui_multipleset.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

MultipleSet::MultipleSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultipleSet)
{
    ui->setupUi(this);
}

MultipleSet::~MultipleSet()
{
    delete ui;
}

void MultipleSet::on_pb_saveInfo_clicked()
{
    if(!judgeSettingInfo())
    {
        return;
    }
}

void MultipleSet::on_pb_start_clicked()
{

}

void MultipleSet::on_pb_stop_clicked()
{

}

void MultipleSet::slt_clearTextEdit()
{
    ui->te_showMsg->clear();
}

bool MultipleSet::judgeSettingInfo()
{
    if(ui->le_ip1->text().trimmed().isEmpty() ||
            ui->le_ip2->text().trimmed().isEmpty() ||
            ui->le_ip3->text().trimmed().isEmpty() ||
            ui->le_ip4->text().trimmed().isEmpty())
    {
        QMessageBox::information(this,"提示","请输入正确Ip地址。","确定");
        return false;
    }

    for(int i=0;i<ui->table_Info->rowCount();++i)
    {
        if(ui->table_Info->item(i,1)->text() == getIpAddr())
        {
            QMessageBox::information(this,"提示","该ip地址已存在，请重新输入。","确定");
            return false;
        }
    }

    return true;
}

QString MultipleSet::getIpAddr()
{
    QString ip = ui->le_ip1->text().trimmed()+"."+ui->le_ip2->text().trimmed()+"."
            +ui->le_ip3->text().trimmed()+"."+ui->le_ip4->text().trimmed();
    return ip;
}


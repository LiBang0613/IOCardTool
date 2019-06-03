#include "multipleset.h"
#include "ui_multipleset.h"

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

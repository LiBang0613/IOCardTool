#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_daqSetPage = new DaqSet(this);
    m_multipltSetPage = new MultipleSet(this);
    ui->tabWidget->addTab(m_daqSetPage,QStringLiteral("数采卡测试"));
    ui->tabWidget->addTab(m_multipltSetPage,QStringLiteral("设备测试"));

    //    m_tcpSocket = new QTcpSocket;
    //    m_tcpSocket->connectToHost("192.168.80.121",502,QTcpSocket::ReadWrite);
    //    connect(m_tcpSocket,SIGNAL(connected()),this,SLOT(slt_connected()));
    //    connect(m_tcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(slt_recvSocketState(QAbstractSocket::SocketState)));
    //    connect(m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slt_readyRead()));

    //    qDebug()<<QString::number(0x0F,16);
    //    int i = 0x00;
    //    qDebug()<<i<;
    //    char* pBuf;
    //    int i;

    //    for (i = 0; i < 5; i++)
    //        array[i] = 0x00;

    //        array[i++] = 0x08;
    //        array[i++] = 0x01;
    //        array[i++] = 0x0F;	// 命令。写多线圈Byte 0: FC = 0F (hex)

    //        array[i++] = 0x00;	// 起始线圈高八位Byte 1-2: Reference number
    //        array[i++] = 0x00;	// 低 八位8bit 一组

    //        array[i++] = 0x00;	// Byte 3-4: Bit count (1-800)
    //        array[i++] = 0x02; //寄存器数量

    //        array[i++] = 0x01;	// Byte 5: Byte count (B = (bit count + 7)/8)

    //        array[i++] = 0x00;	// 字节数据 - 位写数据。二进制表示开关，0x03就是0011表示第一个第二个阀打开

}

MainWindow::~MainWindow()
{
//    delete m_tcpSocket;
    delete ui;
}

void MainWindow::slt_connected()
{
    qDebug()<<"============";
    m_tcpSocket->write(array);
}

void MainWindow::slt_readyRead()
{
    qDebug()<<m_tcpSocket->readAll()<<"====";
    QByteArray responseArray;
    responseArray = m_tcpSocket->readAll();
    //<S> 00  00  00  00  00  06  01  01  00  00  00  10
    //<R> 00  00  00  00  00  05  01  01  02  03  00

    // 1. 接收长度校验
    if ( responseArray.length() != 0x05 + 0x05 + 0x01)
        return; // 接收不正确

    // 2. 功能码+数据长度校验
    if ( responseArray[6].operator !=(0x01)
         || responseArray[7].operator !=(0x01)
         || responseArray[8].operator !=(0x02)
         )
    { // 接收不正确
        return;
    }
    //
    uchar cLow = responseArray[9];
    uchar cHigh = responseArray[10];

    ushort m_wDOState;
    m_wDOState = cLow + (cHigh << 8);
}

void MainWindow::slt_recvSocketState(QAbstractSocket::SocketState state)
{
    qDebug()<<state;
}

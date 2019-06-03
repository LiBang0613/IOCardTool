#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "daqset.h"
#include "multipleset.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slt_connected();
    void slt_readyRead();
    void slt_recvSocketState(QAbstractSocket::SocketState);

private:
    Ui::MainWindow *ui;
    QTcpSocket* m_tcpSocket;
    QByteArray array;

    DaqSet* m_daqSetPage;
    MultipleSet* m_multipltSetPage;
};

#endif // MAINWINDOW_H

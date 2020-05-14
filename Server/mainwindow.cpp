#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>
#include "message.h"
#include "udpsocket.h"
#include "udpserversocket.h"
#include "server.h"
#include "time.h"
#include <QDebug>
#include <thread>
using namespace std;
Server s;
QSettings status1;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    s.ServerInit(3009);
    connect(&s,SIGNAL(updateLog()),this,SLOT(updateLog()));
    ui->textEdit->setText("Server is initialized...\nReady to receive requests.\n");
}

MainWindow::~MainWindow()
{
    QSettings err;
    err.setValue("status","");
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    std::thread Server(&MainWindow::NonGUI, this);
    Server.detach();
}

void MainWindow::NonGUI()
{
    while (1)
        s.serveRequest();
}

void MainWindow::updateLog()
{
    qDebug() << status1.value("status").toString();
    ui->textEdit->setText(ui->textEdit->toPlainText() + status1.value("status").toString());
    status1.setValue("status","");
}

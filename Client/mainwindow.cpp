#include <QSignalMapper>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include "clientt.h"
#include "udpsocket.h"
#include <iostream>
#include <string>
#include <fstream>
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <chrono>
#include <QGraphicsItem>
#include <QPixmap>
#include <QStringListModel>
#include "showimage.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QDir>
#include <QSettings>
#include <requsername.h>

Client c;
bool first;
int op;
QSqlDatabase db;
int accept2;
QSettings shared;

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->pushButton_6->setHidden(true);
   // ui->lineEdit_6->setHidden(true);
    first = true;
    connect(&c,SIGNAL(askClient()),this,SLOT(askClient()));
    connect(&c,SIGNAL(usernameTaken()),this,SLOT(askForNewUsername()));
    connect(&c,SIGNAL(updateLog()),this,SLOT(updateLog()));

    c.chosen_op = 1;
    ui->pushButton_5->setHidden(true);
    ui->radioButton->setHidden(true);
    ui->radioButton_2->setHidden(true);
    ui->radioButton_3->setHidden(true);
    ui->radioButton_4->setHidden(true);
    ui->label_2->setHidden(true);
    ui->pushButton_2->setHidden(true);

    ui->pushButton_2->setHidden(true);


    DatabaseConnect();
    DatabaseInit();

    ui->label_4->setHidden(true);
    ui->lineEdit_2->setHidden(true);
    ui->lineEdit_3->setHidden(true);
    ui->lineEdit_4->setHidden(true);
    ui->lineEdit_5->setHidden(true);
    ui->pushButton_3->setHidden(true);
    ui->pushButton_4->setHidden(true);

    op=1;

}

void MainWindow::updateLog()
{
    qDebug() << c.shared.value("status").toString();
    ui->LSLog->setText(ui->LSLog->toPlainText() + c.shared.value("status").toString());
    c.shared.setValue("status","");
}

void MainWindow::DatabaseConnect()
{
    const QString DRIVER("QSQLITE");
    if (QSqlDatabase::isDriverAvailable(DRIVER))
    {
        db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName("/Users/malaksadek/Desktop/Pics/alogram.db");
        if (!db.open())
            qWarning() << "Database Connect ERROR" << db.lastError().text();
    }
    else
        qWarning() << "No driver" << DRIVER;
}

void MainWindow::DatabaseInit()
{
    if (db.tables().contains(QLatin1String("Alogram"))) {first = false;}
    else {first=true;
    QSqlQuery query("CREATE TABLE Alogram(picture TEXT, owner TEXT,address TEXT, port TEXT, views TEXT, data BLOB, UNIQUE(picture))");

    if (!query.isActive())
        qWarning() << "DATABASE INIT ERROR" << query.lastError().text();

    QSqlQuery query2("CREATE TABLE Alogram_LS(picture TEXT, sent_to TEXT)");
    if (!query2.isActive())
        qWarning() << "DATABASE INIT ERROR" << query2.lastError().text(); }
}

MainWindow::~MainWindow()
{
    std::string username = ui->lineEdit->text().toStdString();
    qDebug() << QString::fromStdString(username);
    if(c.handle("q") == 0) {c.runClient("q",username);};
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QSqlQuery q;
    std::string option = "";
    char string[10] = "localhost";
    int port = 8000;
    c.ClientInit(string, port);
    qDebug() << QString::number(port);
    ui->label_12->setText(QString::number(port));
    c.runServer();
    ui->label->setHidden(true);

    ui->lineEdit->setHidden(true);
    ui->pushButton->setHidden(true);
    ui->label_3->setText("Client is running!");
    ui->pushButton_2->setHidden(false);
    ui->radioButton->setHidden(false);
    ui->radioButton->setChecked(true);
    ui->radioButton_2->setHidden(false);
    ui->radioButton_3->setHidden(false);
    ui->radioButton_4->setHidden(false);
    ui->label_2->setHidden(false);

    std::string username = ui->lineEdit->text().toStdString();
    if (c.handle("0") == 1) {
        char* cl= new char[5];
        cl[0] = 'c';
        c.runClient(cl,username);
    }

    QDir fldr("/Users/malaksadek/Desktop/Pics");
    fldr.setNameFilters(QStringList()<<"*.jpg");
    QStringList list = fldr.entryList();
    for (int i =0;i<list.size();i++)
    {
        if (list.at(i) != "." && list.at(i) != ".." && list.at(i) != "alogram.db") {

        q.prepare("INSERT INTO Alogram(picture,owner,address,port,views,data) VALUES(:pic,:owner,:address,:port,:views,:blob)");
        q.bindValue(":pic",list.at(i));
        q.bindValue(":owner",QString::fromStdString(username));
        q.bindValue(":address",QVariant(-1).toString());
        q.bindValue(":port",QVariant(-1).toString());
        q.bindValue(":views",QVariant(-1).toString());
        q.bindValue(":blob",QVariant(-1).toString());
        /*if (!q.exec())
        {
            qDebug() << "Add Error";
        } */}
    }
    list.removeAt(0);
    list.removeAt(0);
    for (int i =0;i<list.size();i++)
    {
        if (list.at(i) == "alogram.db")
            list.removeAt(i);
    }
    QSqlQuery qq("SELECT picture FROM Alogram WHERE owner LIKE 'NotMe'");
    while (qq.next())
    {
        list << qq.value(0).toString() + "*";
    }

    ui->listView_2->setHidden(false);
    QStringListModel *model = new QStringListModel();
    model->setStringList(list);
    ui->listView_2->setModel(model);
}

void MainWindow::askClient()
{
    ui->pushButton_3->setHidden(false);
    ui->pushButton_4->setHidden(false);
    if (c.shared.value("type").toString() == "views") {ui->label_7->setText(c.shared.value("label").toString());
    }
    c.shared.setValue("type","");
}



void MainWindow::on_pushButton_2_clicked()
{
    char* address = new char[20];
    int port = 0;
    std::string client_list = "";
    std::string picture = ui->lineEdit_4->text().toStdString();
    int numberOfViews = ui->lineEdit_5->text().toInt();
    shared.setValue("pic",QString::fromStdString(picture));
    switch (c.chosen_op) {
        case 1: {
            client_list = c.requestClientList();
            QString temp = QString::fromStdString(client_list).toLatin1().data();
            ui->ClientLog->setText(temp);
            ui->pushButton_5->setHidden(false);
            break;
        }
        case 2: {
            c.choosePeer(ui->lineEdit_2->text().toLatin1().data(),ui->lineEdit_3->text().toInt());
            std::string list = c.requestPictureList();
            QString qlist = QString::fromStdString(list);
            QStringList qlist2 = qlist.split("-");
            for (int i =0;i<qlist2.size();i++)
            {
                qDebug() << "ENTERED LOOP";
                if (qlist2.at(i) == "alogram.db") {
                    qlist2.removeAt(i); qDebug() << "FOUND IT"; }
            }
            QStringListModel *model = new QStringListModel();
            model->setStringList(qlist2);
            ui->listView->setModel(model);
            break;
        }
        case 3: {
            c.shared.setValue("address",ui->lineEdit_2->text());
            c.shared.setValue("port",ui->lineEdit_3->text());
            c.choosePeer(ui->lineEdit_2->text().toLatin1().data(),ui->lineEdit_3->text().toInt());
            c.requestPicture(picture,numberOfViews);
            ui->pushButton_5->setHidden(false);
            break;
        }
        case 4: {
            c.choosePeer(ui->lineEdit_2->text().toLatin1().data(),ui->lineEdit_3->text().toInt());
            c.requestViews(picture,numberOfViews);
            break;
        }
    }
}

void MainWindow::on_radioButton_clicked()
{
        c.chosen_op = 1; ui->label_4->setHidden(true);
        ui->lineEdit_2->setHidden(true);
        ui->lineEdit_3->setHidden(true);
        ui->lineEdit_4->setHidden(true);
        ui->lineEdit_5->setHidden(true);
}

void MainWindow::on_radioButton_2_clicked()
{
       c.chosen_op = 2; ui->label_4->setHidden(false);
       ui->lineEdit_2->setHidden(false);
       ui->lineEdit_3->setHidden(false);
       ui->lineEdit_4->setHidden(true);
       ui->lineEdit_5->setHidden(true);
}

void MainWindow::on_radioButton_3_clicked()
{
        c.chosen_op = 3; ui->label_4->setHidden(false);
        ui->lineEdit_2->setHidden(false);
        ui->lineEdit_3->setHidden(false);
        ui->lineEdit_4->setHidden(false);
        ui->lineEdit_5->setHidden(false);

}

void MainWindow::on_radioButton_4_clicked()
{
        c.chosen_op = 4; ui->label_4->setHidden(false);
        ui->lineEdit_2->setHidden(false);
        ui->lineEdit_3->setHidden(false);
        ui->lineEdit_4->setHidden(false);
        ui->lineEdit_5->setHidden(false);
}

void MainWindow::on_pushButton_5_clicked()
{
    ShowImage si;
    connect(&si,SIGNAL(requestViews()),this,SLOT(request1View()));
    si.setModal(true);
    si.setData(ui->lineEdit_4->text() + ".jpg");
    si.exec();
}

void MainWindow::on_pushButton_3_clicked() // Accept
{
    c.setAnswer(1);
    ui->pushButton_3->setHidden(true);
    ui->pushButton_4->setHidden(true);
    ui->label_7->setHidden(true);
}

void MainWindow::on_pushButton_4_clicked() // Refuse
{
    c.setAnswer(0);
    ui->pushButton_3->setHidden(true);
    ui->pushButton_4->setHidden(true);
    ui->label_7->setHidden(true);
}

void MainWindow::on_listView_2_doubleClicked(const QModelIndex &index)
{
    ShowImage si;
    connect(&si,SIGNAL(requestViews()),this,SLOT(request1View()));
    si.setModal(true);
    si.setData(index.data().toString().replace("*",""));
    si.exec();
}

void MainWindow::request1View()
{
    qDebug() << "Will request 1 view for " << shared.value("expiredv").toString();
    QString pic_tu = shared.value("expiredv").toString();
    QSqlQuery qq2;
    qq2.prepare("SELECT address,port FROM Alogram WHERE owner LIKE 'NotMe' AND picture = ?");
    qq2.addBindValue(shared.value("expiredv").toString());
    qq2.exec();
    qq2.first();
    int start;
    for (int i  =0;i<pic_tu.size();i++)
    {
        if (pic_tu.at(i) == '.') {
            start = i;
        break;}
    }
    qDebug() << "LAST TIME: " << pic_tu.remove(start,pic_tu.length()-start);
    c.choosePeer(qq2.value(0).toString().toLatin1().data(),qq2.value(1).toInt());
    c.requestViews(pic_tu.remove(start,pic_tu.length()-start).toStdString(),1);
}

void MainWindow::askForNewUsername()
{
    std::string username = ui->lineEdit->text().toStdString();
    qDebug() << QString::fromStdString(username);
    if(c.handle("q") == 0) {c.runClient("q",username);};
    qDebug() << "BYKHOSH?";
    reqUsername ru;
    ru.setModal(true);
    if (ru.exec() == 1)
    {
        std::string usernameN = ru.getNewUser();
        //qDebug() << "YA SHEDDA: " << QString(ru.getNewUser());
        if (c.handle("0") == 1) {
            char* cl= new char[5];
            cl[0] = 'c';
            c.runClient(cl,usernameN);
        }
    }
}

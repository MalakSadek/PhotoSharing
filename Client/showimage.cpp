#include "showimage.h"
#include "ui_showimage.h"
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QtSql>
#include <QSqlQuery>
#include <fstream>
#include "message.h"
#include "basen.hpp"
QSettings temp;
#define BYTESIZE 5000000

ShowImage::ShowImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowImage)
{
    ui->setupUi(this);
}


void ShowImage::setData(const QString &pic)
{
    QSqlQuery qu;
    QSqlQuery qu2;

    qu.prepare("SELECT owner FROM Alogram WHERE picture = ?");
    qu.addBindValue(pic);
    if (!qu.exec())
    {
        qDebug() << "Error fetching owner!";
    }
    qu.first();
    qDebug() << "Who's the owner?" << qu.value(0).toString();
    if (qu.value(0).toString() == "NotMe") {

        qu2.prepare("SELECT views FROM Alogram WHERE picture = ? AND owner = ?");
        qu2.addBindValue(pic);
        qu2.addBindValue("NotMe");
        if (!qu2.exec())
        {
            qDebug() << "Error fetching owner!";
        }
        qu2.first();
        ui->label->setText("Note: this picture is not yours! You have " + qu2.value(0).toString() + " views left!");
        if (qu2.value(0).toInt() == 0)
        {
            ui->label->setText("Note: Your number of views is ZERO! An automatic request for 1 view has been made for you!");
            temp.setValue("expiredv",pic);


            QGraphicsScene *scene = new QGraphicsScene();
            ui->graphicsView->setScene(scene);
            QImage image("/Users/malaksadek/Desktop/Pics/.default.jpg");
            QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
            ui->graphicsView->scene()->addItem(item);
            ui->graphicsView->show();

            emit requestViews();
        }

        if (qu2.value(0).toInt() !=0) {

    std::ofstream outfile("/Users/malaksadek/Desktop/Pics/.output.jpg", std::ios::binary);
    QSqlQuery qe;
    qe.prepare("SELECT data FROM Alogram WHERE picture = ? AND owner = ?");
    qe.addBindValue(pic);
    qe.addBindValue(qu.value(0).toString());
    if (!qe.exec())
    {
        qDebug() << "Error fetching data!";
    }
    qe.first();
    int start;
    QString pic2 = qe.value(0).toString();
    for (int i =0;i<50;i++)
    {
        if (pic2.at(i) == '='){
            start = i;
            break;
        }
    }
    std::string sorty = pic2.remove(0,start).toStdString();
    qDebug() << "BEFORE DECODE64";
    bn::decode_b64(sorty.begin(), sorty.end(), std::ostream_iterator<char>(outfile, ""));
    outfile.close();
    qDebug() << "AFTER DECODE64";
    QGraphicsScene *scene = new QGraphicsScene();
    ui->graphicsView->setScene(scene);
    QImage image("/Users/malaksadek/Desktop/Pics/.output.jpg");
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    ui->graphicsView->scene()->addItem(item);
    ui->graphicsView->show();

    QSqlQuery q;
    q.prepare("SELECT views FROM Alogram WHERE picture = ? AND owner = ?");
    q.addBindValue(pic);
    q.addBindValue("NotMe");
    if (!q.exec())
    {
        qDebug() << "Error fetching views!";
    }
    q.first();

    int views = q.value(0).toInt();
    qDebug() << "Views:" << views;

    QSqlQuery q2;
    q2.prepare("UPDATE Alogram SET views = ? WHERE picture = ? AND owner = 'NotMe'");
    q2.addBindValue(QVariant(views-1).toString());
    q2.addBindValue(pic);
    if (!q2.exec())
        qDebug() << "Error updating views"; }}
    else
    {
        QGraphicsScene *scene = new QGraphicsScene();
        ui->graphicsView->setScene(scene);
        QImage image("/Users/malaksadek/Desktop/Pics/" + pic);
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
        ui->graphicsView->scene()->addItem(item);
        ui->graphicsView->show();
    }
    qDebug() << "END OF SHOWIMAGE!";
}

ShowImage::~ShowImage()
{
    QFile file ("/Users/malaksadek/Desktop/Pics/.output.jpg");
    if (file.exists()) {
    file.remove(); }
    delete ui;
}

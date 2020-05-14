#ifndef REQUSERNAME_H
#define REQUSERNAME_H

#include <QDialog>
#include <condition_variable>
#include <mutex>
#include <cstring>
#include "message.h"
#include <arpa/inet.h>
#include <QDir>
#include <QDebug>
#include "udpserversocket.h"
#include <stdio.h>
#include <thread>
#include <QObject>
#include <QtSql>
#include <QSqlQuery>
#include <QDialog>
#include "udpclientsocket.h"
#include <algorithm>
#include "basen.hpp"

namespace Ui {
class reqUsername;
}

class reqUsername : public QDialog
{
    Q_OBJECT

public:
    explicit reqUsername(QWidget *parent = 0);
    struct sockaddr_in mServerSocketAddress, mClientSocketAddress, yourSocketAddress;
    ~reqUsername();
    std::string getNewUser();
    int cnct = 1;
private slots:
    void on_pushButton_clicked();
signals:
    void usernameTaken();
private:
    UDPClientSocket udpSocket;
    Ui::reqUsername *ui;
};

#endif // REQUSERNAME_H

#include "requsername.h"
#include "ui_requsername.h"
#include "udpclientsocket.h"
#include <QSettings>

reqUsername::reqUsername(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reqUsername)
{
    ui->setupUi(this);
}

reqUsername::~reqUsername()
{
    delete ui;
}
static int sockc = socket(AF_INET, SOCK_DGRAM, 0);
void reqUsername::on_pushButton_clicked()
{
    struct hostent *host;
    yourSocketAddress.sin_family  =  AF_INET;
    if((host = gethostbyname("localhost"))== NULL){
        printf("Unknown host name.\n");
    }
    yourSocketAddress.sin_addr = *(struct in_addr *) (host->h_addr);
    yourSocketAddress.sin_port = htons(3009);
   // shared.setValue("status","Connecting to server for first time.\n");
   // emit updateLog();
    std::string username1 = getNewUser();
    cnct = 0;
    char* message2 = new char[3];
    char msg[100];
    msg[0] = 'c';
    msg[1] = '-';
    int j = 0;
    for (int i = 2; i < 100; i++) {
        if (i < username1.length()+2) {
            msg[i] = username1.at(j);
            j++;
        }
        else msg[i] = '\0';
    }
    Message con(0,0,msg,strlen(msg),0,0,0,0,0);
    con.marshal(1);
    udpSocket.writeToSocket(sockc, con.getMessage(), (unsigned)strlen(con.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
    char* message1 = new char[100];
           udpSocket.readFromSocketWithTimeout(sockc, message1, con.getMessage(), (unsigned)strlen(con.getMessage()), 1000, 0, yourSocketAddress, sizeof(yourSocketAddress), 20);


           char *output = NULL;//NULL;
                 output = strstr (message1,"bm8");

                 while (output != NULL) {
                     if (strcmp(output, "bm8")) {
                         emit usernameTaken();
                         return;
                         cnct = 0;
                         char msg[100];
                         msg[0] = 'c';
                         msg[1] = '-';
                         int j = 0;
                         for (long i = 2; i < 100; i++) {
                             if (i < username1.length()+2) {
                                 msg[i] = username1.at(j);
                                 j++;
                             }
                             else msg[i] = '\0';
                         }
                         Message con(0,0,msg,strlen(msg),0,0,0,0,0);
                         con.marshal(1);
                         udpSocket.writeToSocket(sockc, con.getMessage(), (unsigned)strlen(con.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));

                         udpSocket.readFromSocketWithTimeout(sockc, message2, con.getMessage(), (unsigned)strlen(con.getMessage()), 1000, 0, yourSocketAddress, sizeof(yourSocketAddress), 20);
                         char *output = NULL;
                         output = strstr (message2,"bm8");
                     }
                 }
                done(1);
}

std::string reqUsername::getNewUser()
{
    return ui->lineEdit->text().toStdString();
}



//
//  Client.h
//  DistributedProject1
//
//  Created by Malak Sadek on 10/27/17.
//  Copyright © 2017 Malak Sadek. All rights reserved.
//

#ifndef Client_h
#define Client_h

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
using namespace std;
static int sockc = socket(AF_INET, SOCK_DGRAM, 0);
static int socks = socket(AF_INET, SOCK_DGRAM, 0);

#define BYTESIZE 3000000
#define PSIZE 8192

class Client : public QObject{
    Q_OBJECT
private:
    UDPClientSocket udpSocket;
    UDPServerSocket
    udpSSocket;
    int history[100];
    int accept;
    std::string username1;
signals:
    void askClient();
    void usernameTaken();
    void updateLog();
public:
    void setUsername(std::string string)
    {
        username1 = string;
        std::unique_lock<std::mutex> lk(m2);
        is_ready2 = true;
        cv2.notify_one();
    }
    QSettings shared;

    std::mutex m;
    std::condition_variable cv;
    bool is_ready = false;

    std::mutex m2;
    std::condition_variable cv2;
    bool is_ready2 = false;

    std::string UniqueId = "2390";
    int requestCount = 0;
    clock_t t1, t2;
    int cnct = 1;
    int requestID;
    struct sockaddr_in mServerSocketAddress, mClientSocketAddress, yourSocketAddress;
    int chosen_op;
    Client(){}
    void ClientInit(char* computer, int port){
        t1 = clock();
        for (int i = 0; i < 100; i++)
            history[i] = 0;
        sockaddr_in* p;
        p = udpSocket.initializeClient(sockc, mClientSocketAddress, yourSocketAddress, port, computer);
        mClientSocketAddress = p[0];
        yourSocketAddress = p[1];
        mServerSocketAddress = udpSSocket.initializeServer(socks, port+1, mServerSocketAddress);
        //runServer();    //fix this
        //Send client list message to server
    }

    void runClient(char* string, std::string username){
        std::thread client(&Client::doOperationClient, this, string,username);
        //std::thread client(&Client::doOperationServer, this);
        client.join();

    }

    void runServer(){
        //do operation server
        std::thread server(&Client::doOperationServer, this);
        server.detach();
    }


    /*** CLIENT FUNCTIONS START HERE

     doOperationClient(){

     C1. handle
     C2. updateConnection
     C3. connect
     C4. Quit
     C5. selectOperation
     C6. checkReplySource
     C7. choosePeer
     C8. REQUEST
        [1]. requestClientList
        [2]. requestPictureList
        [3]. requestPicture
        [4]. requestViews

     }


     ***/

    int handle(std::string option) {
        //Handle with non blocking receive server's connected clients broadcast

        ifstream infile;
        // I CHANGED HERE!
        //printf("What do you want to send?\nPicture[0]\nQuit [q]\n");

        //std::cin >> option;
        if (option != "\n") {
            if (option=="q") {
                Message M(0, 1, &option.at(0), strlen(&option.at(0)), requestID, 0, 0 ,0,0);
                M.marshal(1);
                qDebug() << "Client is closing!";
                return 0;

            }

            if (option == "0") {
                return 1;
            }
        }
    }

    int updateConnection() {
        struct hostent *host;
        yourSocketAddress.sin_family  =  AF_INET;
        if((host = gethostbyname("localhost"))== NULL){
            printf("Unknown host name.\n");
        }
        yourSocketAddress.sin_addr = *(struct in_addr *) (host->h_addr);
        yourSocketAddress.sin_port = htons(3000);
        char message1 [50] = "";
        udpSSocket.ServerReadFromSocketWithTimeout(sockc, message1, strlen(message1), yourSocketAddress, sizeof(struct sockaddr), 10);
        if (strcmp(message1,"Updating Connected Clients")) {
            std::cout << "Updating Connection to Server.\n";
            udpSSocket.writeToSocket(sockc, message1, strlen(message1), 0, yourSocketAddress, sizeof(struct sockaddr));
            return 1;
        }
        return 0;
    }

    void setAnswer(int ans)
    {
        accept = ans;
        std::unique_lock<std::mutex> lk(m);
        is_ready = true;
        cv.notify_one();
    }

    void connect(std::string username) {
        struct hostent *host;
        yourSocketAddress.sin_family  =  AF_INET;
        if((host = gethostbyname("localhost"))== NULL){
            printf("Unknown host name.\n");
        }
        yourSocketAddress.sin_addr = *(struct in_addr *) (host->h_addr);
        yourSocketAddress.sin_port = htons(3009);
        shared.setValue("status","Connecting to server for first time.\n");
        emit updateLog();
        username1 = username;
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
          //  delete[] message1;
          //  delete[] message2;
        }

    bool Quit(char* string) {
        if ((string[0]=='q')&&(string[1]=='\0')) {
            Message close(0,0,string,strlen(string),0,0,0,0,0);
            close.marshal(1);
            udpSocket.writeToSocket(sockc, close.getMessage(), (unsigned)strlen(close.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
            return 1;
        }
        else return 0;
    }

    /*int selectOperation(int op) {

        while ((operation != 1)&&(operation != 2)&&(operation != 3)&&(operation != 4)) {
            std::cout << "Enter the operation you wish to perform:\n" <<
            "Client List[1]\n"<<
            "Request Picture List From Peer[2]\n"<<
            "Request Picture From Peer[3]\n"<<
            "Request Views From Peer[4]\n";
            std::cin >> operation;
        }
        return operation;
        operation1 = op;
    }*/

    void checkReplySource(char* message1, Message originalMessage, Message testMessage) {
        while(std::to_string(testMessage.getRPCId()).find(UniqueId) == std::string::npos) {
            std::cout << "Incorrect reply!" << std::endl;
            udpSocket.readFromSocketWithTimeout(sockc, message1, originalMessage.getMessage(), (unsigned)strlen(originalMessage.getMessage()), 1000, 0, yourSocketAddress, sizeof(yourSocketAddress), 20);
            Message M2(message1);
        }
    }

    void choosePeer(char* address, int port) {
        //char* address = new char[20];
        //int port = 0;

        struct hostent *host;
        yourSocketAddress.sin_family  =  AF_INET;
        if((host = gethostbyname(address))== NULL){
            printf("Unknown host name.\n");
        }
        yourSocketAddress.sin_addr = *(struct in_addr *) (host->h_addr);
        yourSocketAddress.sin_port = htons(port);
    }

    std::string requestClientList() {
        char temp[100] = "";
        temp[0] = 'c';
        temp[1] = 'l';
        struct hostent *host;
        yourSocketAddress.sin_family  =  AF_INET;
        if((host = gethostbyname("localhost"))== NULL){
            printf("Unknown host name.\n");
        }
        yourSocketAddress.sin_addr = *(struct in_addr *) (host->h_addr);
        yourSocketAddress.sin_port = htons(3009);
        Message Mop(0, 1, temp, strlen(temp),requestID,0,0,0,0);
        Mop.marshal(1);
        udpSocket.writeToSocket(sockc, Mop.getMessage(), (unsigned)strlen(Mop.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
        char message1[1000] = "";

        if (udpSocket.readFromSocketWithTimeout(sockc, message1, Mop.getMessage(), (unsigned)strlen(Mop.getMessage()), 1000, 0, yourSocketAddress, sizeof(yourSocketAddress), 10)) {
            if ((message1[0] != 'U')&&(message1[1] != 'C')&&(message1[2] != 'C')) {

                Message M2(message1);
            checkReplySource(message1, Mop, M2);

            std::string list = "";
            for (int i = 0; i < strlen(M2.getMessage());i++) {
                if(M2.getMessage()[i] !='-')
                    list+=M2.getMessage()[i];
                else
                    list+="\n";
            }
            return list;
            }
            else {
                updateConnection();
                requestClientList();
            }
    }

    }

    std::string requestPictureList() {
        char temp[100] = "";
        temp[0] = 'p';
        temp[1] = 'i';
        temp[3] = 'c';
        std::string list = "";
        Message Mop(0, 2, temp, strlen(temp),requestID,0,0,0,0);
        Mop.marshal(1);
        udpSocket.writeToSocket(sockc, Mop.getMessage(), (unsigned)strlen(Mop.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
        char message1[1000] = "";

        if (udpSocket.readFromSocketWithTimeout(sockc, message1, Mop.getMessage(), (unsigned)strlen(Mop.getMessage()), 1000, 0, yourSocketAddress, sizeof(yourSocketAddress), 10)) {

            Message M2(message1);

            checkReplySource(message1, Mop, M2);

            //std::string list = "";
            for (int i = 0; i < strlen(M2.getMessage());i++) {
                if(M2.getMessage()[i] !='-')
                    list+=M2.getMessage()[i];
                else
                    list+="\n";
            }
            //cout << "Picture List is:\n"<<list;
        }
        return list;
    }

    void requestPicture(std::string picture, int numberOfViews) {
        char temp[100] = "";
        //int numberOfViews = 0;
        //std::string picture = "";
        //cout << "Please enter the picture's name:\n";
        //cin >> picture;
        //cout << "Please enter the number of views to send:\n";
        //cin >>numberOfViews;

        for (unsigned long i = 0; i < picture.length(); i++)
            temp[i] = picture.at(i);
        Message Mop(0, 3, temp, strlen(temp),requestID,0,0,0,numberOfViews);
        Mop.marshal(1);

        udpSocket.writeToSocket(socks, Mop.getMessage(), (unsigned)strlen( Mop.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
    }

    void requestViews(std::string picture, char views) {
        qDebug() << QString::fromStdString(picture);
        char temp[10] = "";
        temp[0] = 'r';
        //std::string picture = "";
        //cout << "Which pictures do you want views for?\n";
        //cin >> picture;
        //cout << "How many views do you want?\n";
        //cin >> temp[1];
        temp[1] = views;
        int j = 0;
        for (int i = 2; i <picture.length()+2;i++)   {
            temp[i]=picture.at(j);
            j++;
        }

        Message Mop(0, 4, temp, strlen(temp),requestID,0,0,0,0);
        Mop.marshal(1);
        udpSocket.writeToSocket(sockc, Mop.getMessage(), (unsigned)strlen(Mop.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
        char message1[1000] = "";
        int first = 1;
        Message M2(message1);
        while ((M2.getMessage()[0] != '0')&&(M2.getMessage()[0] != '1')&&(first)) {
            first = 0;
            udpSocket.readFromSocketWithTimeout(sockc, message1, Mop.getMessage(), strlen(message1), strlen(Mop.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in), 5);
            Message M2(message1);
        }


        //update views
        std::string temp2;
        for (int i =0;i<strlen(message1);i++)
            temp2+=message1[i];

        if (temp2.find("MQ==") != std::string::npos) {
            std::string tmp = ".jpg";
            QSqlQuery q2;
            q2.prepare("UPDATE Alogram SET views = ? WHERE picture = ? AND owner = ?");
            q2.addBindValue(temp[1]);
            q2.addBindValue(QString::fromStdString(picture+tmp));
            q2.addBindValue("NotMe");
            if (!q2.exec())
                qDebug() << "Error updating views!";
            shared.setValue("status","Views granted!"); emit updateLog(); }
        else {
            shared.setValue("status","Views denied!"); emit updateLog(); }

        qDebug() << "ENDDD";

    }


    void doOperationClient(char* string,std::string username){//sending views, sending a client request
        //sending a picture to add to server, remove from server, or send to client, or send views
        if (!Quit(string)) {

            if (cnct)
                connect(username);

            requestCount++;
            requestID = stoi(UniqueId+to_string(requestCount));

            //int operation = op;
            char temp[2] = "";
            for (int i = 0; i < 2; i++)
                temp[i] = '\0';

            //1 - Request client list
            //2 - Request picture list
            //3 - Request picture
            //4 - Request views

            /*switch (operation) {
                case 1: {
                    chosen_op = 1;
                    //requestClientList();
                    break;
                }
                case 2: {
                    //choosePeer();
                    //requestPictureList();
                    chosen_op = 2;
                    break;
                }
                case 3: {
                    chosen_op = 3;
                    //choosePeer();
                    //requestPicture();
                    break;
                }
                case 4: {
                    //choosePeer();
                    //requestViews();
                    chosen_op = 4;
                    break;
                }
            }*/
        }
    }

    /**SERVER FUNCTIONS START HERE ***

     doOperationServer() {
     S1. clientQuit
     S2. duplicateFiltering
     S3. checkHistory
     S4. addHistory
     S5. extractPayload
     S6. errorCheck
     S7. prepareToReceive
     S8. handlePicture
     S9. receiveFragments
     S10.  SEND
        [1]. sendPacket
        [2]. sendAcknowledgement
        [3]. sendFragments
        [4]. sendPictureList
     }

     ********/

    int clientQuit(char* string) {
        if ((string[0]=='q')&&(string[1]=='\0')) {
            Message close(0,0,string,strlen(string),0,0,0,0,0);
            close.marshal(1);
            udpSocket.writeToSocket(socks, close.getMessage(), (unsigned)strlen(close.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
            return 1;
        }
        return 0;
    }

    void duplicateFiltering(int ID) {
        for (int i = 0; i < 100; i++)
            if (history[i]/10 == ID/10) {
                if (history[i]%10 <= ID%10)
                    history[i] = 0;
                std::cout<<"Reply Acknowledgement Received.\nRemoving from history..."<<std::endl;
                break;
            }
    }

    int checkHistory (int ID) {
        for (int i = 0; i < 100; i++)
            if(history[i] == ID) {
                return 1;
            }
        return 0;
    }

    void addHistory (int ID) {
        for (int i = 0; i < 100; i++)
            if (history[i] == 0) {
                history[i] = ID;
                break;
            }
    }

    char* extractPayload (int index, char* message) {
        char* temp = new char[BYTESIZE];
        int begin = 0;
        int j = 0;
        for (int i = 0; i < strlen(message); i++) {
            if (begin) {
                if (i < strlen(message)) {
                    temp[j] = message[i];
                    j++;
                }
            }
            else {
                if (message[i] == '=')
                    begin = 1;
            }
        }
        //std::cout << temp;
      //  delete []temp;
        return temp;
    }

    char* errorCheck (char* temp, char* message, int total, int count) {
        for (int i = PSIZE; i < PSIZE+20; i++)
            temp[i] = '\0';
        return temp;
    }

    void prepareToReceive(int ID, int op) {
        char reply[50] ="Server Acknowledgement: Client Message Received.\n";
        Message M1(1, op, reply, strlen(reply), ID, 0, 0, 0,0);
        M1.marshal(1);

        udpSSocket.writeToSocket(socks, M1.getMessage(), strlen(M1.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));


    }

    void handlePicture (std::string fragments, int numberOfViews) {

        char* frags = new char[BYTESIZE];
        //std::ofstream outfile("/home/mohapard/Desktop/Desktop/output.jpg", std::ios::binary);

        for (int i = 0; i < BYTESIZE; i++)
            frags[i] = '\0';
        if (fragments.at(0) != 'M') {
            frags[0] = 'M';
            for (unsigned long i = 1; i < fragments.length(); i++)
                frags[i] = fragments.at(i-1);
        }
        else
        {
            for (unsigned long i = 0; i < fragments.length(); i++)
                frags[i] = fragments.at(i);
        }
        QSqlQuery q;

        q.prepare("INSERT INTO Alogram(picture,owner,address,port,views,data) VALUES(:pic,:owner,:address,:port,:views,:blob)");
        q.bindValue(":pic",shared.value("pic").toString()+".jpg");
        q.bindValue(":owner","NotMe");
        q.bindValue(":address",shared.value("address").toString());
        q.bindValue(":port",shared.value("port").toString());
        q.bindValue(":views",QVariant(numberOfViews).toString());
        q.bindValue(":blob",QString::fromStdString(frags));
        if (!q.exec())
        {
            qDebug() << "Add NotMe image";
        }
        /*Message fragmessage(frags);
        std::string output = "";
        for (int i = 0; i < fragmessage.getMessageSize();i++)
            output+=fragmessage.getMessage()[i];*/

        //outfile << output;
    }

    /*void receiveFragments(Message M, char* string) {
        int nov = M.getnOv();
        std::string fragments = "";
        std::string fragheader = "";
        std::string fragheaderpayload = "";
        char* frags = new char [BYTESIZE];
        std::string totalfrags = "";
        std::cout<<"Defragmenting message.\n";
        for (unsigned long m = 0; m<strlen(string);m++) {
            fragheader+= string[m];
        }
        int found = 0;
        for (int n = 0; n < fragheader.length()-1;n++) {
            if (found) {
                fragheaderpayload+=fragheader.at(n+1);
            }
            else {
                if (fragheader.at(n)=='=')
                    found = 1;
            }
        }
        char* message1 = new char [BYTESIZE];

        int y = 1;
        int count = 0;

        while (count <= M.getFragTotal()-1) {
            yourSocketAddress = udpSSocket.readFromSocketWithBlock(socks, message1, BYTESIZE, 0, yourSocketAddress, sizeof(struct sockaddr_in));
            Message Mr (message1);

            frags = extractPayload(0, message1);
            for (int i = 0; i < strlen(frags); i++)
                fragments+=frags[i];
            y++;
            count++;
            prepareToReceive(M.getRPCId(), 0);
            shared.setValue("status",QVariant(count+1).toString() + " Reply received from server.\n");
            emit updateLog();
            sendAcknowledgement(M.getRPCId());
        }
        fragments.erase(std::remove(fragments.begin(), fragments.end(), '='), fragments.end());
        fragments = fragheaderpayload+fragments;

        handlePicture(fragments,nov);
    }*/

    void receiveFragments(Message M, char* string) {
        int nov = M.getnOv();
        std::string fragments = "";
        std::string fragheader = "";
        std::string fragheaderpayload = "";
        char* frags = new char [BYTESIZE];
        std::string totalfrags = "";
        std::cout<<"Defragmenting message.\n";
        for (unsigned long m = 0; m<strlen(string);m++) {
            fragheader+= string[m];
        }
        int found = 0;
        for (int n = 0; n < fragheader.length()-1;n++) {
            if (found) {
                fragheaderpayload+=fragheader.at(n+1);
            }
            else {
                if (fragheader.at(n)=='=')
                    found = 1;
            }
        }
        char* message1 = new char [BYTESIZE];

        int y = 1;
        int count = 0;
        //std::ofstream mrOutFile("/home/mohapard/Desktop/Desktop/mrOutFile.txt", std::ios::binary);

        while (count <= M.getFragTotal()-1) {
            yourSocketAddress = udpSSocket.readFromSocketWithBlock(socks, message1, BYTESIZE, 0, yourSocketAddress, sizeof(struct sockaddr_in));

            Message Mr (message1);

            frags = extractPayload(0, message1);

            for (int i = 0; i < strlen(frags); i++)
                fragments+=frags[i];

            y++;
            count++;
            prepareToReceive(M.getRPCId(), 0);
            shared.setValue("status",QVariant(count-1).toString() + " Reply received from server.\n");
            emit updateLog();
            sendAcknowledgement(M.getRPCId());
        }

        fragments.erase(std::remove(fragments.begin(), fragments.end(), '='), fragments.end());
        fragments = fragheaderpayload+fragments;

        handlePicture(fragments,nov);
    }

    void sendPacket(char* temp, int ID, int total, int count, int numberOfViews) {

        Message M1(1,5, temp, strlen(temp), ID, 1, count, total,numberOfViews);
        M1.marshal(0);
        udpSocket.writeToSocket(socks, M1.getMessage(), (unsigned)strlen(M1.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
    }

    void sendAcknowledgement(int ID) {
        char reply[12] = "Acknowledge";
        Message M4(1,10, reply, strlen(reply),ID, 0, 0, 0,0);
        M4.marshal(1);

        udpSocket.writeToSocket(socks, M4.getMessage(), (unsigned)strlen(M4.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
    }

    void sendFragments(Message M2, int ID, size_t size, int nov) {
        shared.setValue("status","Fragmenting message...\n");
        emit updateLog();
        long sizeleft = strlen(M2.getMessage());
        int index = 0;
        int count = 0;
        int working = 1;
        char* message1 = new char [BYTESIZE];
        yourSocketAddress.sin_family = AF_INET; /* note that this is needed */
        char* temp = new char[PSIZE+20];
        for (int i = 0; i < PSIZE+20; i++)
            temp[i] = '\0';

        int total = (int)(strlen(M2.getMessage())/PSIZE)+1;

        while ((sizeleft > 0)&&(index < strlen(M2.getMessage()))&&(working)) {
            if(M2.getOperation() == 10)//client acknowledgement
                duplicateFiltering(ID);

            if (!checkHistory(ID)) {           //valid request
                addHistory(ID);
                for (int i = 0; i < PSIZE; i++)
                    temp[i] = '\0';

                sizeleft -=PSIZE;
                int j = 0;
                for (int i = index; i < index+PSIZE; i++) {
                    temp[j] = M2.getMessage()[i];
                    j++;
                }

                index+=PSIZE;
                temp = errorCheck(temp, M2.getMessage(), total, count);
                //std::cout << "\nPacket #"<<count<<": ";
                shared.setValue("status", "\nPacket #" + QVariant(count).toString() + ": ");
                emit updateLog();


                sendPacket(temp, ID, total, count, nov);
                count++;

                char *ack = NULL;// NULL;
                int count = 0;
                while ((!ack)&&(count < 10)){
                    int aLength = sizeof(mServerSocketAddress);
                    mServerSocketAddress.sin_family = AF_INET; /* note that this is needed */
                    size_t length = BYTESIZE;
                    if (udpSSocket.ServerReadFromSocketWithTimeout(socks, message1, length, yourSocketAddress, aLength, 10) <=0) {
                            working = 0;
                        break;

                    }
                    ack = strstr (message1,"QWNrbm93bGVkZ2U=");
                    count++;
                }
                if (!working)
                    break;
                    Message M2(message1);
                    if(M2.getOperation() == 10)//client acknowledgement
                        duplicateFiltering(ID);
            }

        }
        sendPacket(temp, ID, total, count, nov);
       // delete [] temp;
    }

    void sendPictureList(int ID) {
        char* temp = new char[50];
        QString tt;
        QDir fldr("/Users/malaksadek/Desktop/Pics");
        fldr.setNameFilters(QStringList()<<"*.jpg");
        QStringList list = fldr.entryList();
        for (int i =0;i<list.size();i++) {
            if (list.at(i) != "." && list.at(i) != ".." && list.at(i) != "")
            {
                tt.append(list.at(i));
                tt.append("-");
            }
             }
        QByteArray ba = tt.toLocal8Bit();
        temp = strdup(ba.constData());
        //temp[0] = 't';
        //temp[1] = 't';
        Message Mop(1, 2, temp, strlen(temp),ID,0,0,0,0);
        Mop.marshal(1);
        udpSocket.writeToSocket(socks, Mop.getMessage(), (unsigned)strlen( Mop.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
    }

    void doOperationServer(){ //sending a picture accept, sending a views accept
        //receive picture, picture request, view request, or views
        while (1) {
            char* string = new char [BYTESIZE];
            int aLength = sizeof(mServerSocketAddress);
            mServerSocketAddress.sin_family = AF_INET; /* note that this is needed */
            size_t length = BYTESIZE;
            yourSocketAddress = udpSSocket.readFromSocketWithBlock(socks, string, length, 0, yourSocketAddress, aLength);

            if ((string[0]=='U')&&(string[1]=='C')&&(string[2]=='C')&&(string[3]=='\0'))
                updateConnection();
            else {
                Message M (string);

                if ((!clientQuit(string))&&(!checkHistory(M.getRPCId()))) {

                    int aLength;

                    aLength = sizeof(yourSocketAddress);
                    Message M(string);
                    char temp[2] = "";
                    for (int i = 0; i < 2; i++)
                        temp[i] = '\0';

                    if (strlen(string) < PSIZE)  {
                        //std::cout << "Request received from client.\n" << std::endl;
                        shared.setValue("status","Request received from client.\n");
                        emit updateLog();
                        if(M.getOperation() == 10) //client acknowledgement
                            duplicateFiltering(M.getRPCId());
                        switch (M.getOperation()) {
                            case 2: { //picture list

                            emit askClient();
                            std::unique_lock<std::mutex> lk(m);
                            while (!is_ready)
                                {
                                    cv.wait(lk);
                                    if (!is_ready)
                                        std::cout << "Answer me man!\n";
                                }
                            if (accept) {
                                sendPictureList(M.getRPCId());}
                            accept = 0;
                            is_ready = false;
                                break;
                            }
                            case 3: { //picture request
                         emit askClient();
                         std::unique_lock<std::mutex> lk(m);
                         while (!is_ready)
                             {
                                 cv.wait(lk);
                                 if (!is_ready)
                                     std::cout << "Answer me man!\n";
                             }

                               if (accept) {
                                    Message M2 = readFile(M.getMessage());
                                    qDebug() << "ANA HENA YA MAAAAANNNN" << M2.getMessage();

                                    if (M2.getMessage() != "This picture does not exist!") {
                                    QSqlQuery q;
                                    q.prepare("INSERT INTO Alogram_LS (picture,sent_to) VALUES (:pic,:sent_to)");
                                    std::string str(M.getMessage());
                                    std::string tmp = str+".jpg";
                                    char* final = new char[20];
                                    strcpy(final,tmp.c_str());
                                    q.bindValue(":pic", final);
                                    char str5[INET_ADDRSTRLEN];
                                    inet_ntop(AF_INET,&(yourSocketAddress.sin_addr),str5,INET_ADDRSTRLEN);
                                    q.bindValue(":sent_to",str5);
                                    q.exec();
                                    sendFragments(M2, M.getRPCId(), strlen(M.getMessage()),M.getnOv());
                                    accept = 0;
                                    is_ready = false;
                                    break;
                               }
                               else {
                                        shared.setValue("status","This picture does not exist!");
                                        emit updateLog();

                                    }
                                }

                            }
                            case 4: {   //views
                                std::string picture = "";

                                for (int i = 2; i < strlen(M.getMessage())+2;i++) {
                                    picture+= M.getMessage()[i];
                                }
                                char ack [2]="";

                                shared.setValue("type","views");
                                std::string p1 = "Client wants ";

                                QString picture1 = QString::fromStdString(picture);
                                QString isa = ".jpg";
                                picture1 = picture1.toUtf8().constData();
                                picture1 = picture1 + isa;
                                int start = 0;

                                for (int i =0;i<picture1.size();i++)
                                {
                                    if (picture1.at(i) == ".")
                                    {
                                        start = i+4;
                                        break;
                                    }
                                }
                                picture1.remove(start,picture1.length()-start);



                                std::string p2 = " views for the picture ";
                                std::string p3 = " Do you accept?";
                                std::string tmp1 = p1 + QVariant(int(M.getMessage()[1])).toString().toStdString() + p2 + picture1.toUtf8().constData() + p3;
                                shared.setValue("label",QString::fromStdString(tmp1));

                                //std::cout << "Client wants " << M.getMessage()[1] << " views for the picture: "<<picture<<"\nDo you accept? [0/1]\n";
                                //cin >> ack[0];
                                emit askClient();
                                std::unique_lock<std::mutex> lk(m);
                                while (!is_ready)
                                    {
                                        cv.wait(lk);
                                        if (!is_ready)
                                            std::cout << "Answer me man!\n";
                                    }

                                QSqlQuery q3;
                                q3.prepare("SELECT sent_to FROM Alogram_LS WHERE picture LIKE '" + picture1 + "%'");
                                q3.exec();
                                q3.first();
                                char str7[INET_ADDRSTRLEN];
                                inet_ntop(AF_INET,&(yourSocketAddress.sin_addr),str7,INET_ADDRSTRLEN);
                                if (accept && (q3.value(0).toString() == str7))
                                {
                                    ack[0] = '1';
                                }
                                else ack[0] = '0';
                                if(ack[0]=='0')
                                    qDebug()<<"Views refused.\n";
                                else {

                                    qDebug()<<"Views given.\n";

                                }
                                Message Mop(1, 6, ack, 1, M.getRPCId(),0,0,0,0);
                                Mop.marshal(1);
                                udpSocket.writeToSocket(socks,  Mop.getMessage(), (unsigned)strlen( Mop.getMessage()), 0, yourSocketAddress, sizeof(struct sockaddr_in));
                                break;
                            }
                        }
                    }
                    else {
                        shared.setValue("status","Reply received from client!\n");
                        emit updateLog();
                        duplicateFiltering(M.getRPCId());
                        switch(M.getOperation()) {
                            case 5: {   //picture
                                if (!checkHistory(M.getRPCId())) {           //valid request
                                    addHistory(M.getRPCId());

                                    sendAcknowledgement(M.getRPCId());
                                    if(M.getFraged()) {
                                        receiveFragments(M, string);
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
               // delete [] string;
            }
        }
    }


    Message readFile(char* pic) {
        bool flag = 1;
        char* content = new char [BYTESIZE];
        std::string option = "";
        ifstream infile;
        std::string picture = "";
        for (int i = 0; i < strlen(pic);i++)
            picture +=pic[i];
        while (flag) {
            try{
                infile.open("/Users/malaksadek/Desktop/Pics/"+picture+".jpg", ios::binary);

                if(infile.is_open())
                {cout<<"Reading File...\n";
                    flag=0;
                }
                else {
                    cout<<"Error\n";
                    flag = 1;
                    char* content = new char [BYTESIZE];
                    content = "This picture does not exist!";
                    Message M(0, 1, content, strlen(content), requestID, 0, 0 ,0,0);
                   // delete []content;
                    return M;
                }

            }
            catch(exception e){
                flag =1;
            };
        }

        int i;

        for(int j = 0; j < BYTESIZE; j++)
            content[j] = '\0';

        for(i=0 ; infile.eof()!=true ; i++) // get content of infile
            content[i]=infile.get();
        i--;

        content[i] = '\0';
        shared.setValue("status",QVariant(i).toString() + "Characters read..\n");
        //cout << i << " Characters read...\n";
        Message M(0, 1, content, i+1, requestID, 0, 0 ,0,0);
        M.marshal(1);
       // delete []content;
        return M;
    }

    sockaddr_in getMyAddress() {
        return udpSocket.getMyAddress();
    }
    ~Client() {
    }
};

#endif /* Client_h */

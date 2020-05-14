//
//  Server.h
//  DistributedProject1
//
//  Created by Malak Sadek on 10/27/17.
//  Copyright © 2017 Malak Sadek. All rights reserved.
//

#ifndef Server_h
#define Server_h
#include "message.h"
#include <iostream>
#include <string>
#include <fstream>
#include <QSettings>
#include "udpserversocket.h"
#include <QDebug>

static int sock = socket(AF_INET, SOCK_DGRAM, 0);
class Server :public QObject{
    Q_OBJECT
signals:
    void updateLog();
private:
    UDPServerSocket udpServerSocket;
    //static int sock;
    int port;
    int history[100];
    struct sockaddr_in connectedClients[5];
    std::string connectedUsernames[5];
    struct sockaddr_in totalClients[5];
    int numberOfConnections;
    int totalConnections;

    void checkClientQuit(char* message1) {
        char *test = NULL;
        test = strstr (message1,"cQ==");
        if(test) {
            for (int i = 0; i < 5; i++) {
                if ((connectedClients[i].sin_port == aSocketAddress.sin_port)&&(connectedClients[i].sin_addr.s_addr== aSocketAddress.sin_addr.s_addr)) {
                    connectedClients[i].sin_port = 2222;
                    connectedClients[i].sin_addr.s_addr= 2222;
                    connectedUsernames[i] = "";
                    numberOfConnections--;
                    //std::cout<<"Client disconnected.\n";

                }
            }
            sp.setValue("status",sp.value("status").toString()+"Client disconnected.\n");
            emit updateLog();
        }
    }

    bool NewClientConnection(char* message1) {
        int found = 0;
        int foundname = 0;
        Message M(message1);
        if((M.getMessage()[0]=='c')&&(M.getMessage()[1]=='-')) {
            for (int j = 0; j < 5; j++) {
                if ((totalClients[j].sin_port == aSocketAddress.sin_port)&&(totalClients[j].sin_addr.s_addr== aSocketAddress.sin_addr.s_addr)) {
                    found = 1;
                }
            }
            if (!found) {
                char* name = new char [20];
                for (int i = 0; i < strlen(M.getMessage())-2; i++)
                    name[i] = M.getMessage()[i+2];



                for (int j = 0; j < 5; j++) {
                    char *output = NULL;
                    char* tempname = new char[20];
                    for (int f = 0; f < connectedUsernames[j].length(); f++)
                        tempname[f] = connectedUsernames[j].at(f);
                    output = strstr (tempname,name);
                    if(output != NULL) {
                        foundname = 1;
                    }
                }
                if (!foundname) {
                for (int i = 0; i < 5; i++) {
                    if ((totalClients[i].sin_port == 4444)&&(totalClients[i].sin_addr.s_addr== 4444)) {
                        totalClients[i].sin_port = aSocketAddress.sin_port;
                        totalClients[i].sin_addr.s_addr= aSocketAddress.sin_addr.s_addr;
                        totalConnections++;
                        for (int j = 2; j < strlen(M.getMessage())+2; j++)
                            connectedUsernames[i] += M.getMessage()[j];
                        sp.setValue("status","Client Added.\n");
                        addClientConnection();
                        char* reply = new char[3];
                        reply[0] = 'y';
                        reply[1] = 'e';
                        reply[2] = 's';
                        Message M1(1, 0, reply, strlen(reply), M.getRPCId(), 0, 0, 0, 0);
                        M1.marshal(1);
                        sendReply(sock, M1.getMessage(), strlen(M1.getMessage()), 0, aSocketAddress, sizeof(struct sockaddr));
                        return 1;
                        }
                    }

                    }
                else {
                    char* reply = new char[3];
                    reply[0] = 'n';
                    reply[1] = 'o';
                    Message M1(1, 0, reply, strlen(reply), 0, 0, 0, 0, 0);
                    M1.marshal(1);
                    sendReply(sock, M1.getMessage(), strlen(M1.getMessage()), 0, aSocketAddress, sizeof(struct sockaddr));
                }
            }
        }
        return 0;
    }

    bool checkCurrentClientConnections() {
        for (int i = 0; i < 5; i++) {
            if((connectedClients[i].sin_port == aSocketAddress.sin_port)&&(connectedClients[i].sin_addr.s_addr== aSocketAddress.sin_addr.s_addr)) {
                return 1;
            }
        }
        return 0;
    }

    void addClientConnection() {

     for (int i = 0; i < 5; i++) {
        if ((connectedClients[i].sin_port == 2222)&&(connectedClients[i].sin_addr.s_addr==2222)) {
            connectedClients[i] = aSocketAddress;
            numberOfConnections++;
            //std::cout<<"Client connected.\n";

            break;
        }
     }
     if (sp.value("status").toString() == "Client Added.\n")
     sp.setValue("status",sp.value("status").toString()+ "Client connected.\n");
     emit updateLog();
  }

    int checkClientType() {
        if (udpServerSocket.getMyAddress().sin_addr.s_addr == udpServerSocket.getPeerAddress().sin_addr.s_addr)
            return 0; //Local client
        else
            return 1; //Remote client
    }

    void updateClientList() {
        char reply[5];
        reply[0]='U';
        reply[1]='C';
        reply[2]='C';
        reply[3]='\0';
        char* message1 = new char [50];

        for (int i = 0; i < 5; i++) {
            if ((connectedClients[i].sin_port != 2222)&&(connectedClients[i].sin_addr.s_addr!=2222))
                udpServerSocket.writeToSocket(sock, reply, strlen(reply), 0, connectedClients[i], sizeof(struct sockaddr));
            if (!udpServerSocket.ServerReadFromSocketWithTimeout(sock, message1, strlen(message1), aSocketAddress, sizeof(struct sockaddr), 10)) {
                connectedClients[i].sin_port = 2222;
                connectedClients[i].sin_addr.s_addr = 2222;
            }
        }
    }

    bool duplicateFilter(int ID) {
        for (int i = 0; i < 100; i++)
            if(history[i] == ID) {
                return 1;
            }
        return 0;
    }

    void addToHistory(int ID) {
        for (int i = 0; i < 100; i++)
        if (history[i] == 0) {
            history[i] = ID;
            break;
        }
    }

    void returnClientList(int ID) {
        int found = 0;
        for (int j = 0; j < 5; j++) {
            qDebug() << connectedClients[j].sin_addr.s_addr;
        if (connectedUsernames[j] != "") {
        std::string list = "";
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j <5; j++) {
                if ((connectedClients[i].sin_addr.s_addr==totalClients[j].sin_addr.s_addr)&&(connectedClients[i].sin_port==totalClients[j].sin_port)) {
                    list += connectedUsernames[j];
                    list +="-";
                    break;
                }
            }
        }
        list.erase(std::remove(list.begin(), list.end(), '\0'), list.end());
        char* reply = new char[1000];
        for (int i = 0; i < list.length(); i++)
            reply[i] = list.at(i);
        Message M1(1, 0, reply, strlen(reply), ID, 0, 0, 0, 0);
        M1.marshal(1);
        sendReply(sock, M1.getMessage(), strlen(M1.getMessage()), 0, aSocketAddress, sizeof(struct sockaddr));
    found = 1;
        }
        }
    if (!found) {

        char* reply = new char[1000];
        reply = "Sorry, there has been a server crash, please restart your application to reconnect.\n";
        Message M1(1, 0, reply, strlen(reply), ID, 0, 0, 0, 0);
        M1.marshal(1);
        sendReply(sock, M1.getMessage(), strlen(M1.getMessage()), 0, aSocketAddress, sizeof(struct sockaddr));
}
    }

    ssize_t getRequest() {
        std::string fragments = "";
        std::string fragheader = "";
        char* message1 = new char [1000000];
        int aLength = 0, n = 0;
        aLength = sizeof(aSocketAddress);
        aSocketAddress.sin_family = AF_INET; /* note that this is needed */
        size_t length = 1000000;

        aSocketAddress = udpServerSocket.readFromSocketWithBlock(sock, message1, length, 0, aSocketAddress, aLength);
        qDebug() << "READ";
        checkClientQuit(message1);


        if (!NewClientConnection(message1)) {
            if (!checkCurrentClientConnections()) {
                addClientConnection();
            }
        }

        //Unmarshal request
        Message M (message1);

        if (checkClientType() == 1) {
            std::cout<<"Client is remote.\n";
        }
        else if (checkClientType() == 0) {
            //std::cout<<"Client is local.\n";
            if (M.getMessageType() == 0) {  //request
                    if (M.getOperation() == 1) {
                        returnClientList(M.getRPCId());
                    }

                //else duplicate request, ignore
            }
        }
         return n;
    }

    void sendReply (int sock, char* reply, size_t replylength, int x, sockaddr_in aSocketAddress, socklen_t addrlength) {
        udpServerSocket.writeToSocket(sock, reply, replylength, 0, aSocketAddress, addrlength);
    }
public:
    struct sockaddr_in mySocketAddress, aSocketAddress;
    int UniqueID = 2983;
    clock_t t1;
    clock_t t2;
    QSettings sp;
    Server(){}
    void ServerInit(int _myPort){
        t1 = clock();
        for (int i = 0; i <100; i++) {
            if (i < 5) {
                connectedClients[i].sin_port = 2222;
                connectedClients[i].sin_addr.s_addr = 2222;
                connectedClients[i].sin_family  =  AF_INET;
                totalClients[i].sin_port = 4444;
                totalClients[i].sin_addr.s_addr = 4444;
                totalClients[i].sin_family  =  AF_INET;
                connectedUsernames[i] = "";
            }
            history[i] = 0;
        }
        port = _myPort;
        numberOfConnections = 0;
        totalConnections = 0;
        mySocketAddress = udpServerSocket.initializeServer(sock, port, mySocketAddress);
    }

    void serveRequest() {
        getRequest();

       // updateClientList();
    }

    sockaddr_in getMyAddress() {
        return udpServerSocket.getMyAddress();
    }

    ~Server() {}
};

#endif /* Server_h */


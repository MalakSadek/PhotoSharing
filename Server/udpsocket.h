//
//  UPDSocket.h
//  DistributedProject1
//
//  Created by Malak Sadek on 10/27/17.
//  Copyright © 2017 Malak Sadek. All rights reserved.
//
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <stdio.h>
#include "message.h"
#ifndef UPDSocket_h
#define UPDSocket_h

class UDPSocket
{
    protected:
    int mysock;
    int peersock;
    sockaddr_in peerAddr;
    sockaddr_in myAddress;
    int myPort;
    int peerPort;
    bool enabled;
public:
    UDPSocket () {};

    sockaddr_in initializeServer (int sock, int port, sockaddr_in sa) {
        this->mysock = sock;
        this->myPort = port;
        sa.sin_family  =  AF_INET;
        sa.sin_port = htons(port);
        sa.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(sock, (const sockaddr*)&sa, sizeof(struct sockaddr_in))!= 0){
            std::cerr<<"Could not bind server socket, the problem is: "<< strerror(errno) << std::endl;
            close(sock);
            return sa;
        }
        this->myAddress = sa;
        return sa;
    }

    sockaddr_in* initializeClient(int sock, sockaddr_in mSocketAddress, sockaddr_in yourSocketAddress, int port, char computer[50]) {
        this->peersock = sock;
        this->peerPort = port;

        mSocketAddress.sin_family  =  AF_INET;
        mSocketAddress.sin_port = htons(0);
        mSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);

        if(bind(sock, (const sockaddr*) &mSocketAddress, sizeof(struct sockaddr_in))!= 0){
            std::cerr<<"Could not bind client socket, the problem is: "<< strerror(errno) << std::endl;
            close(sock);
            //return &mSocketAddress;
        }
        this->peerAddr = mSocketAddress;

        struct hostent *host;
        yourSocketAddress.sin_family  =  AF_INET;
        if((host = gethostbyname(computer))== NULL){
            printf("Unknown host name.\n");
        }
        yourSocketAddress.sin_addr = *(struct in_addr *) (host->h_addr);
        yourSocketAddress.sin_port = htons(port);

        sockaddr_in* p;
        p = new sockaddr_in[2];
        p[0] = mSocketAddress;
        p[1] = yourSocketAddress;
        return p;
    }

    void writeToSocket(int sock, char reply[1000000], size_t replylength, int x, sockaddr_in aSocketAddress, socklen_t addrlength) {
            ssize_t n;
        if((n = sendto(sock, reply, (unsigned)strlen(reply)+1, 0, (struct sockaddr*)&aSocketAddress,addrlength))<0) perror("Writing to socket failed.\n");
           else return;
    }

    int readFromSocketWithTimeout(int sock, char message1[1000000], char message2[1000000], size_t message2length, size_t length, int x, sockaddr_in aSocketAddress, int aLength, int timeout) {
        int retransmit = 0;
        struct timeval tv;
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            std::cerr << "Could not set receive timer!\n";
        }
        ssize_t n;

        while(((n = recvfrom(sock, message1, length, 0, (struct sockaddr*) &aSocketAddress, (socklen_t*)&aLength))<0)&&(retransmit<10)) {
            writeToSocket(sock, message2, message2length, 0, aSocketAddress, (socklen_t)sizeof(aSocketAddress));
            retransmit++;
            std::cout << "Did not receive server acknowledgement, retrying in 10 seconds...\n[Retransmission #"<<retransmit<<"]"<<std::endl;
        }
        if (retransmit == 10) {
            std::cout << "Retransmitted 10 times, message dropped.\n";
            return 0;
        }
        else
            printf("Message sent!\n");

        tv.tv_sec = 0;
        tv.tv_usec = 0;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            std::cerr << "Could not set socket back to blocking!\n";
        }
        return 1;
    }

    int ServerReadFromSocketWithTimeout(int sock, char message1[1000000], size_t length, sockaddr_in aSocketAddress, int aLength, int timeout) {
        struct timeval tv;
        tv.tv_sec = timeout;
        tv.tv_usec =0;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            std::cerr << "Could not set receive timer!\n";
        }
        ssize_t n;

        if((n = recvfrom(sock, message1, length, 0, (struct sockaddr*) &aSocketAddress, (socklen_t*)&aLength))<0) {
            std::cout << "No acknowledgement received, aborting operation...\n";
            return 0;
        }
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
            std::cerr << "Could not set socket back to blocking!\n";
        }

        return 1;
    }

    int index = 0;
    sockaddr_in readFromSocketWithBlock(int sock, char message1[1000000], size_t length, int x, sockaddr_in aSocketAddress, int aLength) {
        ssize_t n;
        if((n = recvfrom(sock, message1, length, 0, (struct sockaddr*) &aSocketAddress, (socklen_t*)&aLength))<0) {
            //perror("Read with block.") ;
        }
        return aSocketAddress;
    }


    int getMyPort() {
        return myPort;
    };

    int getMySocketHandler() {
        return mysock;
    };

    int getPeerSocketHandler() {
        return peersock;
    };

    sockaddr_in getMyAddress() {
        return myAddress;
    };

    sockaddr_in getPeerAddress() {
        return peerAddr;
    };

    ~UDPSocket(){
        close(mysock);
        close(peersock);
    };
};

#endif /* UPDSocket_h */

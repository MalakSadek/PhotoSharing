//
//  UDPClientSocket.h
//  DistributedProject1
//
//  Created by Malak Sadek on 10/27/17.
//  Copyright © 2017 Malak Sadek. All rights reserved.
//

#ifndef UDPClientSocket_h
#define UDPClientSocket_h
#include "udpsocket.h"
#include <QObject>

#define BYTESIZE 3000000

class UDPClientSocket: public UDPSocket{//}, public QObject {
public:
    UDPClientSocket(){}
    sockaddr_in* initializeClient (int sock, sockaddr_in mSocketAddress, sockaddr_in yourSocketAddress, int port, char computer[50]){
        return UDPSocket::initializeClient(sock, mSocketAddress, yourSocketAddress, port, computer);
    }

    void writeToSocket (int sock, char message[BYTESIZE], size_t msglength, int x, sockaddr_in yourSocketAddress, socklen_t addrlength) {
        UDPSocket::writeToSocket(sock, message, msglength, x, yourSocketAddress, addrlength);
    }

    int readFromSocketWithTimeout(int sock, char reply[BYTESIZE], char message[BYTESIZE], size_t messagelength, size_t length, int x, struct sockaddr_in yourSocketAddress, int aLength, int timeout) {

        return UDPSocket::readFromSocketWithTimeout(sock, reply, message, messagelength, length, x, yourSocketAddress, aLength, timeout);
    }

    ~UDPClientSocket(){}
};

#endif /* UDPClientSocket_h */

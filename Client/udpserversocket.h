//
//  UDPServerSocket.h
//  DistributedProject1
//
//  Created by Malak Sadek on 10/27/17.
//  Copyright © 2017 Malak Sadek. All rights reserved.
//

#ifndef UDPServerSocket_h
#define UDPServerSocket_h
#include <udpsocket.h>
#include <QObject>

#define BYTESIZE 3000000

class UDPServerSocket :public UDPSocket {//{//:public UDPSocket, public QObject {
public:
    UDPServerSocket() {};
    sockaddr_in initializeServer (int sock, int port, sockaddr_in sa) {
        return UDPSocket::initializeServer(sock, port, sa);
    }
    void writeToSocket (int sock, char reply[BYTESIZE], size_t replylength, int x, sockaddr_in aSocketAddress, socklen_t addrlength) {
        UDPSocket::writeToSocket(sock, reply, replylength, x, aSocketAddress, addrlength);
    }

    sockaddr_in readFromSocketWithBlock(int sock, char message1[BYTESIZE], size_t length, int x, struct sockaddr_in aSocketAddress, int aLength) {

        return UDPSocket::readFromSocketWithBlock(sock, message1, length, x, aSocketAddress, aLength);
    }

    int ServerReadFromSocketWithTimeout(int sock, char message1[BYTESIZE], size_t length, sockaddr_in aSocketAddress, int aLength, int timeout) {
        return UDPSocket::ServerReadFromSocketWithTimeout(sock, message1, length, aSocketAddress, aLength, timeout);
    }

    sockaddr_in getMyAddress() {
        return UDPSocket::getMyAddress();
    }

    sockaddr_in getPeerAddress() {
        return UDPSocket::getPeerAddress();
    }

    ~UDPServerSocket() {}
};
#endif /* UDPServerSocket_h */

//
// Created by candr on 04-Feb-22.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#pragma once
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <iterator>

class Server {
public:
    Server(const char* serverIP, int port);
    ~Server();
    bool StartServer();
    bool ListenForClient();
    bool rpcProcess();
    void ParseTokens(char* buffer, std::vector<std::string>& a);

private:
    int rpcCount;
    int server_fd;
    int socketID;
    int port;
    struct sockaddr_in address;

    bool rpcConnect(std::vector<std::string>& arrayTokens);
    bool rpcDisconnect();
    bool rpcStatus();
    bool validateLogin();
};


#endif //SERVER_SERVER_H

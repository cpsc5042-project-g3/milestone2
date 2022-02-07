//
// Created by candr on 04-Feb-22.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#pragma once
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <iterator>

using namespace std;

class Server {
public:
    Server(int port);
    ~Server();
    bool StartServer();
    bool ListenForClient();
    bool rpcProcess();
    static void ParseTokens(char* buffer, std::vector<std::string>& a);

private:
    int server_fd;
    int socketID;
    int port;
    struct sockaddr_in address;

    bool rpcConnect(std::vector<std::string>& arrayTokens) const;
    bool rpcDisconnect() const;
    static bool rpcStatus();
    static bool validLogin(const string& userName, const string& password);
};


#endif //SERVER_SERVER_H

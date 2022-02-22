/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 1
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Date: Feb 2022
 */

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
    bool startServer();
    bool connectWithClient();

private:
    const int BUFFER_SIZE = 1024;
    int server_fd;
    int socketID;
    int port;
    struct sockaddr_in address{};

    static void * myThreadFun(void* vargp);
};


#endif //SERVER_SERVER_H

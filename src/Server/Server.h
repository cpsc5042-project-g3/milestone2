/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 12 Mar 2022
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
    explicit Server(int port);
    ~Server();
    bool startServer();
    bool connectWithClient();

private:
    const int BUFFER_SIZE = 1024;       // max buffer size
    int server_fd;                      // listening socket descriptor
    int socketID;                       // connection socket descriptor for each client
    int port;                           // PORT number
    struct sockaddr_in address{};       // address struct

    static void * myThreadFun(void* vargp);
};

#endif //SERVER_SERVER_H

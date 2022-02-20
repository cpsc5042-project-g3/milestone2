/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 1
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Date: Feb 2022
 */

#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "Server.h"
#include "RPCImpl.h"

using namespace std;

Server::Server(int portIn) {
    server_fd = 0;   // "listening" socket descriptor
    socketID = 0;    // "connection" socket descriptor
    port = portIn;
}

Server::~Server() = default;

// Create server "listening" socket
bool Server::startServer() {
    int opt = 1;
    const int BACKLOG = 10;
    cout << ">> Starting server." << endl;

    // Create Socket: "listening" socket
    cout << ">> Creating server socket." << endl;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror(">> Error: Socket creation failed");
        exit(EXIT_FAILURE);
    }
    cout << ">> Socket created successfully." << endl;

    // (Optional step) Helps in reuse of address and port.
    cout << ">> Set socket options." << endl;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror(">> Error: Setsockopt failed.");
        exit(EXIT_FAILURE);
    }

    // Specifies the communication domain for "address"
    address.sin_family = AF_INET;
    // Specifies IP address for "address"
    address.sin_addr.s_addr = INADDR_ANY;
    // Specifies PORT number for "address"
    address.sin_port = htons(port);

    // Bind "address" with Socket (server_fd)
    cout << ">> Binding IP address and PORT with Socket." << endl;
    if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror(">> Error: Binding failed");
        exit(EXIT_FAILURE);
    }
    cout << ">> Bind successful." << endl;

    // Start listening
    cout << ">> Listening process started." << endl;
    if (listen(server_fd, BACKLOG) < 0) {
        perror(">> Error: Listening process failed");
        exit(EXIT_FAILURE);
    }
    cout << ">> Listening for client to connect." << endl;
    return true;
}

// Create client-server "connection" socket
bool Server::connectWithClient() {
    int addrLen = sizeof(address);

    vector<pthread_t> threadIDList;

    for (;;) {
        cout << "ZZZZ" << endl;

        // Create "connection" socket and start accepting
        cout << ">> Accept process started." << endl << endl;
        if ((socketID = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrLen)) < 0) {
            perror(">> Error: Connection failed");
            exit(EXIT_FAILURE);
        }
        cout << ">> Client successfully connected." << endl;
        cout << ">> Waiting for client RPC." << endl;
        cout << ">> Buffer size available: " << BUFFER_SIZE << " bytes. " << endl << endl;

        // TODO: Launch thread to process RPC ???
        pthread_t thread_id;
        threadIDList.push_back(thread_id);
        int connectSocket = socketID;
        pthread_create(&thread_id, NULL, myThreadFun, (void*)&connectSocket);

    }
    return true;
}

// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void* Server::myThreadFun(void* vargp) {

    sleep(1);

    int socket = *(int *) vargp;
    printf("PPPP\n");
    auto *rpcImplObj = new RPCImpl(socket);
    rpcImplObj->rpcProcess();   // This will go until client disconnects;
    printf("Done with Thread");

    return NULL;

}
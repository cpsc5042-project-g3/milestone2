/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 12 Mar 2022
 */

#include "Server.h"
#include "RPCImpl.h"
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <vector>
#include <iterator>
#include <iostream>

using namespace std;

/*
 * Server constructor.
 */
Server::Server(int portIn) {
    server_fd = 0;   // "listening" socket descriptor
    socketID = 0;    // "connection" socket descriptor
    port = portIn;
}

/*
 * Server destructor.
 */
Server::~Server() = default;

/*
 * Create server "listening" socket
*/
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
    cout << ">> Listening successful." << endl;
    return true;
}

/*
 * Create client-server "connection" socket
 */
bool Server::connectWithClient() {
    int addrLen = sizeof(address);

    vector<pthread_t> threadIDList;

    cout << ">> Accept process started. Waiting for clients..." << endl;
    for (;;) {
        // Create "connection" socket and start accepting

        if ((socketID = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrLen)) < 0) {
            perror(">> Error: Connection failed");
            exit(EXIT_FAILURE);
        }
        printf("\n>>>>>>>>> Client %d successfully connected.\n", socketID - 3);
        printf(">> Waiting for RPCs from Client %d.\n", socketID - 3);
        cout << ">> Buffer size available: " << BUFFER_SIZE << " bytes. " << endl;

        pthread_t thread_id;
        threadIDList.push_back(thread_id);
        int clientID = socketID;
        if (pthread_create(&thread_id, nullptr, myThreadFun, (void*)&clientID) != 0) {
            perror(">> Error: Thread creation unsuccessful.");
            return false;
        }
    }
    return true;
}

/*
 * This function is executed as a thread when its name is specified in pthread_create()
 */
void* Server::myThreadFun(void* vargp) {

    sleep(1);

    int clientID = *(int *) vargp;
    printf(">> Client %d thread started.\n", clientID - 3);
    auto *rpcImplObj = new RPCImpl(clientID);
    rpcImplObj->rpcProcess();   // This will go until client disconnects;
    printf(">> Client %d done with thread.\n", clientID - 3);
    printf("<<<<<<<<< Client %d session ended.\n", clientID - 3);
    cout << "\n>> Waiting for next client." << endl;
    return nullptr;

}




/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 10 Mar 2022
 */

#include <iostream>
#include "Server.h"

using namespace std;

int main(int argc, char const *argv[]) {
    int port = atoi(argv[2]);            // PORT number: has to match with client
    bool statusOk;
    auto *serverObj = new Server(port);

    // Create server listening socket
    statusOk = serverObj->startServer();

    // Create server-client connection socket
    while (statusOk) {
        statusOk = serverObj->connectWithClient();
    }

    delete serverObj;

    return 0;
}
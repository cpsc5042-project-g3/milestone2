/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 1
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Date: Feb 2022
 */
#include <iostream>
#include "Server.h"

using namespace std;

int main(int argc, char const *argv[]) {
    int port = atoi(argv[2]);            // PORT number: has to match with client
    bool statusOk;
    auto *serverObj = new Server(port);

    // Create server listening socket
    if (serverObj->startServer())
        statusOk = true;

    // Create server-client connection socket
    while (statusOk) {

        // Once connected with the client, start accepting RPC
        if (serverObj->connectWithClient()) {

            // Start interaction with client.
            // This will be ongoing until client sends a "Disconnect" RPC
            serverObj->rpcProcess();

            cout << "\n>> Client session ended." << endl;
            cout << ">> Waiting for next client." << endl;
        }
    }

    delete serverObj;

    return 0;
}
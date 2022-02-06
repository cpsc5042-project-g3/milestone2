//
// Created by candr on 04-Feb-22.
//

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <iterator>
#include <iostream>

#include "Server.h"

using namespace std;

Server::Server(const char *serverIP, int portIn) {
    rpcCount = 0;
    serverIP = (char*) serverIP;
    port = portIn;
}

Server::~Server() {}

bool Server::StartServer() {
    int opt = 1;
    const int BACKLOG = 10;
    cout << ">> Starting server." << endl;

    // Create socket
    cout << ">> Creating socket" << endl;
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if ((server_fd == 0)) {
        perror(">> Socket creation failed");
        exit(EXIT_FAILURE);
    }
    cout << ">> Socket created successfully." << endl;

    // Attach socket to port
    cout << ">> Attaching socket to port." << endl;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror(">> Socket attachment failed");
        exit(EXIT_FAILURE);
    }
    cout << ">> Socket attached successfully." << endl;

    // Not sure what exactly this does, but it IS required so it stays.
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind port
    cout << ">> Binding port." << endl;
    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
        perror("Port binding failed");
        exit(EXIT_FAILURE);
    }
    cout << ">> Bind successful." << endl;

    // Start listening
    cout << ">> Listening process started." << endl;
    if (listen(server_fd, BACKLOG) < 0) {
        perror("Listening process failed");
        exit(EXIT_FAILURE);
    }
    cout << ">> Waiting for client." << endl << endl;

    return true;
}

bool Server::ListenForClient() {
    int addrLen = sizeof(address);

    if ((socketID = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrLen)) < 0) {
        perror("Client listening error.");
        exit(EXIT_FAILURE);
    }
    cout << ">> Client successfully connected." << endl;
    cout << ">> Waiting for client RPC." << endl << endl;

    this->rpcProcess();
    cout << ">> End of client RPC processing." << endl;
    return true;
}

void Server::ParseTokens(char *buffer, std::vector<std::string> &a) {
    char* token;
    char* rest = (char*) buffer;

    cout << ">> Parsing tokens." << endl;
    while ((token = strtok_r(rest, ";", &rest))) {
        a.emplace_back(token);
    }
}

bool Server::rpcProcess()
{
    const char* rpcs[] = { "connect", "disconnect", "status"};
    char buffer[1024] = { 0 };
    std::vector<std::string> arrayTokens;
    int valread = 0;
    bool connected = false;
    bool statusOk = true;
    bool continueOn = true;
    const int RPCTOKEN = 0;


    while ((continueOn) && (statusOk))
    {
        // Should be blocked when a new RPC has not called us yet
        if ((valread = read(this->socketID, buffer, sizeof(buffer))) <= 0)
        {
            printf("errno is %d\n", errno);
            break;
        }
        cout << ">> Client RPC received: ";
        printf("%s\n", buffer);

        arrayTokens.clear();
        this->ParseTokens(buffer, arrayTokens);

        // Enumerate through the tokens. The first token is always the specific RPC
        cout << ">> Token(s) received: ";
        for (auto t = arrayTokens.begin(); t != arrayTokens.end(); ++t) {
            printf("\n\ttoken = %s", t);
        }
        cout << endl;

        // string statements are not supported with a switch, so using if/else logic to dispatch
        string aString = arrayTokens[RPCTOKEN];

        if (!connected && (aString == "connect")) {
            statusOk = rpcConnect(arrayTokens);  // Connect RPC
            if (statusOk) {
                connected = true;
            }
        }
        else if (connected && (aString == "disconnect")) {
            statusOk = rpcDisconnect();
            continueOn = false; // We are going to leave this loop, as we are done
        }
        else if (connected && (aString == "status")) {
            statusOk = rpcStatus();   // Status RPC
        }
        else {
            cout << ">> Unrecognized RPC." << endl;
            // Not in our list, perhaps, print out what was sent
        }
    }
    return true;
}

bool Server::rpcConnect(std::vector<std::string> & arrayTokens) {
    cout << ">> Confirm RPC: Connect" << endl << endl;

    const int USERNAMETOKEN = 1;
    const int PASSWORDTOKEN = 2;

    // Strip out tokens 1 and 2 (username, password)
    string userNameString = arrayTokens[USERNAMETOKEN];
    string passwordString = arrayTokens[PASSWORDTOKEN];
    char szBuffer[80];

    // Our Authentication Logic. Looks like Mike/Mike is only valid combination
    if ((userNameString == "MIKE") && (passwordString == "MIKE")) {
        strcpy(szBuffer, "1;"); // Connected
    } else {
        strcpy(szBuffer, "0;"); // Not Connected
    }

    // Send Response back on our socket
    int nlen = strlen(szBuffer);
    szBuffer[nlen] = 0;
    send(this->socketID, szBuffer, strlen(szBuffer) + 1, 0);

    return true;
}

bool Server::rpcStatus() {
    cout << ">> Confirm RPC: Status" << endl << endl;
    return true;
}

bool Server::rpcDisconnect() {
    cout << ">> Confirm RPC: Disconnect" << endl << endl;
    char szBuffer[16];
    strcpy(szBuffer, "disconnect");

    // Send Response back on our socket
    int nlen = strlen(szBuffer);
    szBuffer[nlen] = 0;
    send(this->socketID, szBuffer, strlen(szBuffer) + 1, 0);
    return true;
}

bool Server::validateLogin() {}
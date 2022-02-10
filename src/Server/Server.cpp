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
#include "Server.h"
#include <sstream>
#include <map>

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

    // Create "connection" socket and start accepting
    cout << ">> Accept process started." << endl << endl;
    if ((socketID = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrLen)) < 0) {
        perror(">> Error: Connection failed");
        exit(EXIT_FAILURE);
    }
    cout << ">> Client successfully connected." << endl;
    cout << ">> Waiting for client RPC." << endl;
    cout << ">> Buffer size available: " << BUFFER_SIZE << " bytes. " << endl << endl;

    return true;
}

// Process all the RPCs
bool Server::rpcProcess() {
    vector<string> arrayTokens;
    ssize_t msgByte;
    bool continueOn = true;
    const int RPCTOKEN = 0;

    while ((continueOn)) {
        char *buffer= new char[BUFFER_SIZE];
        // Blocked until a RPC is sent to server
        msgByte = read(socketID, buffer, BUFFER_SIZE);

        if (msgByte <= 0) {
            perror(">> Error: Read failed");
            delete[] buffer;
            break;
        }

        // Once an RPC is received
        cout << ">> Client RPC received: " << buffer << endl;
        cout << ">> Total bytes: " << msgByte << endl;

        // Parse and Print the tokens
        arrayTokens.clear();
        parseTokens(buffer, arrayTokens);
        printToken(arrayTokens);

        // Get RPC name: string statements are not supported with a switch, so using if/else logic to dispatch
        string rpcName = arrayTokens[RPCTOKEN];

        if (rpcName == "connect") {  // This step is actually to authenticate user
            rpcConnect(arrayTokens);
        } else if (rpcName == "disconnect") {
            rpcDisconnect();
            continueOn = false; // We are going to leave this loop, as we are done
        } else if (rpcName == "status") {
            rpcStatus();   // Status RPC
        } else {
            string error = "Error: Invalid request";
            cerr << ">> " << error << endl;
            sendResponse(&error[0]);
            // Not in our list, perhaps, print out what was sent
        }
        delete[] buffer;
    }
    return true;
}

// Parse incoming message
void Server::parseTokens(char* buffer, vector<string> &a) {
    char *token;

    cout << ">> Parsing tokens." << endl;
    while ((token = strtok_r(buffer, ";", &buffer))) {
        a.emplace_back(token);
    }
}

// Print incoming message
void Server::printToken(vector<string> &arrayTokens) {
    // Enumerate through the tokens. The first token is always the specific RPC name
    cout << ">> Token(s) received: ";
    for (auto &arrayToken : arrayTokens) {
        printf("\n\ttoken = %s", arrayToken.c_str());
    }
    cout << endl;
}

// Send response to client
bool Server::sendResponse(char *message) {
    return send(socketID, message, strlen(message) + 1, 0);
}

// RPC: Connect/Login
bool Server::rpcConnect(vector<string> &arrayTokens) {
    cout << ">> Processing RPC: Connect" << endl << endl;
    cout << ">> Validating login info." << endl;

    const int USERNAMETOKEN = 1;
    const int PASSWORDTOKEN = 2;

    // Strip out tokens 1 and 2 (username, password)
    string userNameString = arrayTokens[USERNAMETOKEN];
    string passwordString = arrayTokens[PASSWORDTOKEN];

    // Our Authentication Logic.
    return validLogin(userNameString, passwordString);

}

// RPC: Other PRCs
bool Server::rpcStatus() {
    cout << ">> Processing RPC: Status" << endl << endl;
    return true;
}

// RPC: Disconnect
bool Server::rpcDisconnect() {
    cout << ">> Processing RPC: Disconnect" << endl << endl;
    // Send Response back on our socket
    string response = "Disconnect successful.";
    if (sendResponse(&response[0]))
        cout << ">> Informed client disconnect successful.";

    return true;
}

/*
 * This function validates the supplied login credentials against a list of
 * approved users.  Normally, we would expect approved users to be maintained
 * in a database somewhere, but we are simulating this by maintaining these
 * in a simple text file.
 */
bool Server::validLogin(const string &userName, const string &password) {
    string line, name, pwd;
    auto users = new map<string, string>();

    // open file of approved users
    ifstream inFile("ApprovedUsers.txt");
    if (!inFile.is_open()) {
        cout << ">> Failed to read approved users list." << endl;
        return false;
    }

    // read approved users and their passwords line by line
    while (inFile.peek() != EOF) {
        getline(inFile, line);

        if (line.length() > 1) {
            stringstream ss(line);
            ss >> name >> pwd;
            users->insert(make_pair(name, pwd));
        }
    }
    inFile.close();

    // validate supplied userName and password
    auto iter = users->find(userName);
    if (iter == users->end()) {
        // user name not found
        string response = "Invalid user name.";
        cout << ">> " << response << endl << endl;
        sendResponse(&response[0]);
        return false;
    } else if (iter->second == password) {
        // found matching user name AND password
        string response = "User name and password validated.";
        cout << ">> " << response << endl << endl;
        sendResponse(&response[0]);
        return true;
    } else {
        // user name found, but password does not exist
        string response = "User name found, but password does not match.";
        cout << ">> " << response << endl << endl;
        sendResponse(&response[0]);
        return false;
    }

}
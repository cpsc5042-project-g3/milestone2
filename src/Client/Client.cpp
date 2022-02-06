

#include "Client.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

using namespace std;

Client::Client() {
    userName = (char*) "TBD";
    password = (char*) "TBD";
    socketID = 0;
    buffer = nullptr;
    connected = false;
}

Client::~Client() {
    close(socketID);
}

bool Client::ConnectServer(const char* serverIP, int port) {
    struct sockaddr_in serv_addr;
    buffer = {0};

    // assemble connect message
    stringstream ss;
    ss << "connect;" << userName << ";" << password << ";";
    string temp = ss.str();
    char* rpcConnect = &temp[0];

    // Create socket
    cout << ">> Creating socket." << endl;
    socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID < 0) {
        cout << "Socket creation failed." << endl;
        return false;
    }
    cout << ">> Socket creation successful." << endl;

    // Do mystery magic
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    cout << ">> Validating IP address." << endl;
    if (inet_pton(AF_INET, serverIP, &serv_addr.sin_addr) <= 0) {
        cout << ">> IP address is invalid or not supported." << endl;
        return false;
    }
    cout << ">> IP address is valid." << endl;

    // Open connection to server
    cout << ">> Connecting to server." << endl;
    if (connect(socketID, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << ">> Connection failed." << endl;
        return false;
    }
    cout << ">> Connection successful" << endl;

    // use buffer to send message to server
    cout << ">> Sending rpcConnect message to server." << endl;
    buffer = rpcConnect;
    int nlen = strlen(buffer);
    buffer[nlen] = 0;   // Put the null terminator

    // write message to server
   int valwrite = send(socketID, buffer, strlen(buffer)+1, 0);
   printf(">> Connect message sent with %d bytes\n", valwrite);

   // read response from server
   int valread = read(socketID, buffer, 1024);
   printf(">> Server response: %s with valread=%d\n", buffer, valread);

   connected = true;
   return true;
}

bool Client::queryTrait(const char* trait, const char* traitValue) {}

bool Client::guessName(const char *name) {}

bool Client::eliminatePerson(const char *name) {}

bool Client::DisconnectServer() const {
    const char* logoffRPC = "disconnect";

    if (connected) {
        // add logoff message to buffer
        strcpy(buffer, logoffRPC);
        int nlen = strlen(buffer);
        buffer[nlen] = 0;   // Put the null terminator

        // write buffer to server
        int valwrite = send(socketID, buffer, strlen(buffer) + 1, 0);
        printf(">> Disconnect message sent with %d bytes\n", valwrite);

        // read response from serfver
        int valread = read(socketID, buffer, 1024);
        printf(">> Server response: %s with valread=%d\n", buffer, valread);
    }
    else {
        cout << ">> Disconnect not required. No server connection detected." << endl;
    }
    return true;
}

bool Client::setUserName(char* userNameIn) {
    userName = userNameIn;
    return true;
}

bool Client::setPassword(char* passwordIn) {
    password = passwordIn;
    return true;
}
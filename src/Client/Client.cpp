/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 1
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Date: Feb 2022
 */
#include "Client.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

using namespace std;

Client::Client() {
    socketID = 0;          // server-client "connection" socket descriptor
    connected = false;
    response = nullptr;
}

Client::~Client() {
    close(socketID);
    delete userName;
    delete password;
}

bool Client::connectServer(const char *serverIP, int port) {
    struct sockaddr_in serv_addr{};

    // Create Socket: "connection" socket
    cout << "\n>> Starting to create a client socket." << endl;
    socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID < 0) {
        perror(">> Error: Socket creation failed");
        return false;
    }
    cout << ">> Socket creation successful." << endl;

    // Specifies the communication domain for "server address"
    serv_addr.sin_family = AF_INET;
    // Specifies the PORT number for "server address"
    serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    cout << ">> Validating IP address." << endl;
    if (inet_pton(AF_INET, serverIP, &serv_addr.sin_addr) <= 0) {
        perror(">> Error: IP address is invalid or not supported");
        return false;
    }
    cout << ">> IP address is valid." << endl;

    // Open connection to server
    cout << ">> Connecting to server." << endl;
    if (connect(socketID, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror(">> Error: Connection failed");
        return false;
    }
    cout << ">> Connection to server successful." << endl << endl;

    connected = true;
    return true;
}

// RPC: Connect/LogIn
bool Client::logIn() {
    stringstream ss;
    string temp1, messageTitle;

    // prompt for userName
    if (!getUserName()) {
        return false;
    }
    // prompt for password
    if (!getPassword()) {
        return false;
    }
    // Assemble login message to server
    ss << "connect;" << userName << ";" << password << ";";
    ss >> temp1;
    int n = temp1.length();
    char temp2[n + 1];
    strcpy(temp2, temp1.c_str());
    messageTitle = "Login";

    // Send login message to server and get response
    if (sendMessage(messageTitle, temp2)) {
        if (getResponse()) {
            if (strcmp(response, "User name and password validated.") == 0) {
                cout << ">> You are now logged in." << endl;
                return true;
            }
        }
    }
    return false;
}

bool Client::getUserName() {
    userName = new char[MAX_LEN];
    cin.clear();
    cout << ">> Enter valid username: ";
    cin >> userName;
    if (strlen(userName) > MAX_LEN || cin.fail()) {
        cin.clear();
        cerr << ">> Error: Username too long." << endl;
        cout << endl;
        delete userName;
        return false;
    }
    return true;
}

bool Client::getPassword() {
    password = new char[MAX_LEN];
    cin.clear();
    cout << ">> Enter the associated password: ";
    cin >> password;
    if (strlen(password) > MAX_LEN || cin.fail()) {
        cin.clear();
        cerr << ">> Error: Password too long." << endl;
        cout << endl;
        delete password;
        return false;
    }
    return true;
}

// RPCs to be implemented later
bool Client::queryTrait(const char *trait, const char *traitValue) {}

bool Client::guessName(const char *name) {}

bool Client::eliminatePerson(const char *name) {}

// RPC: Disconnect
bool Client::disconnectServer() {
    // Assemble disconnect message
    string temp = "disconnect";
    char *logoffRPC = &temp[0];

    // Send disconnect message to server and get response
    if (connected)
        if (sendMessage("Disconnect", logoffRPC))
            if (getResponse())
                if (strcmp(response, "Disconnect successful.") == 0) {
                    cout << ">> Now you are logged off and disconnected." << endl;
                    return true;
                }
    return false;
}

// Send message to server
bool Client::sendMessage(const string &title, char *message) {
    cout << "\n>> Sending " << title << " message to server." << endl;

    // Assemble message
    size_t nlen = strlen(message);
    message[nlen] = 0;   // Put the null terminator
    if (send(socketID, message, strlen(message) + 1, 0)) {
        cout << ">> " << title << " message sent." << endl;
        return true;
    }
    cout << ">> " << title << " message failed to send." << endl;
    return false;
}

// Get response from server
bool Client::getResponse() {
    char buffer[1024] = { 0 };
    if (read(socketID, buffer, 1024)) {
        cout << ">> Server response: " << buffer << endl;
        response = buffer;
        return true;
    }
    return false;
}
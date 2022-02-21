/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 10 Mar 2022
 */
#include "Client.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>

using namespace std;

/*
 * This is the constructor for the Client object.
 */
Client::Client() {
    socketID = 0;          // server-client "connection" socket descriptor
    buffer = nullptr;
    connected = false;
}

/*
 * This is the destructor for the client object.  It also closes the socket
 * associated with the client.
 */
Client::~Client() {
    close(socketID);
}

/*
 * This function opens a socket connection with the server identified
 * by serverIP and port.
 */
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

/*
 * This function connects to the server using userName and password and
 * returns true if the connection was successful, i.e. userName and password
 * were valid.
 */
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
            if (strcmp(buffer, "User name and password validated.") == 0) {
                cout << ">> You are now logged in." << endl;
                return true;
            }
        }
    }
    return false;
}

/*
 * This function prompts the user to enter their user name and verifies that
 * it is of an acceptable length.
 */
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

/*
 * This function prompts the user to enter their password and verifies that
 * it is within an acceptable length.
 */
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

/*
 * This function transmits a character query to the server
 */
bool Client::queryTrait(const char *trait, const char *traitValue) {}

/*
 * This function submits a guess to the server about the target characters name.
 */
bool Client::guessName(const char *name) {}

/*
 * This function submits a command to the server to eliminate a character from the game
 */
bool Client::eliminatePerson(const char *name) {}

/*
 * This function submits a disconnect command to the server.
 */
bool Client::disconnectServer() {
    // Assemble disconnect message
    string temp = "disconnect";
    char *logoffRPC = &temp[0];

    // Send disconnect message to server and get response
    if (connected)
        if (sendMessage("Disconnect", logoffRPC))
            if (getResponse())
                if (strcmp(buffer, "Disconnect successful.") == 0) {
                    cout << ">> Now you are logged off and disconnected." << endl;
                    return true;
                }
    return false;
}

/*
 * This function submits a message to the server.  It is used by other functions after they
 * generate the detailed contents of their particular messages.
 */
bool Client::sendMessage(const string &title, char *message) {
    cout << "\n>> Sending " << title << " message to server." << endl;

    // Assemble message
    buffer = message;
    size_t nlen = strlen(buffer);
    buffer[nlen] = 0;   // Put the null terminator
    if (send(socketID, buffer, strlen(buffer) + 1, 0)) {
        cout << ">> " << title << " message sent." << endl;
        return true;
    }
    cout << ">> " << title << " message failed to send." << endl;
    return false;
}

/*
 * This function listens for a response message from the server.  It is used in conjunction
 * with a message submitted to the server to validate that the message was received.
 */
bool Client::getResponse() {
    if (read(socketID, buffer, 1024)) {
        cout << ">> Server response: " << buffer << endl;
        return true;
    }
    return false;
}
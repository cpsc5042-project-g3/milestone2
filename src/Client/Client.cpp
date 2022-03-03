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
    connected = false;
    userName = nullptr;
    password = nullptr;
}

/*
 * This is the destructor for the client object.  It also closes the socket
 * associated with the client.
 */
Client::~Client() {
    close(socketID);
    delete userName;
    delete password;
}

/*
 * This function opens a socket connection with the server identified
 * by serverIP and port and sends a "RPC-connect" to server
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
    printf(">> Socket creation successful. You are Client %d.\n", socketID);

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
    char buffer[1024] = { 0 };

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
        // Get server response
        if (read(socketID, buffer, 1024)) {
            cout << ">> Server response: " << buffer << endl;
            if (strcmp(buffer, "User name and password validated.") == 0) {
                printf(">> %s, you are now logged in.\n", userName);
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
 * This function submits a "RPC-getCharacterNames" to server and gets back a string vector
 * of character names, which will be kept and updated locally.
 */
bool Client::getCharacterNamesFromServer() {
    char buffer[1024] = { 0 };
    string temp = "getCharacterList";
    char *getlistRPC = &temp[0];
    if (connected)
        if (sendMessage("Get Character List", getlistRPC))
            // Get server response
            if (read(socketID, buffer, 1024)) {
                cout << ">> Character list received." << endl;
                cout << ">> Making a local copy." << endl;
                parseTokens(buffer);
                return true;
                }
    return false;

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
 * This function submits a "RPC-disconnect" to the server.
 */
bool Client::disconnectServer() {
    char buffer[1024] = { 0 };
    // Assemble disconnect message
    string temp = "disconnect";
    char *logoffRPC = &temp[0];

    // Send disconnect message to server and get response
    if (connected)
        if (sendMessage("Disconnect", logoffRPC))
            // Get server response
            if (read(socketID, buffer, 1024)) {
                cout << ">> Server response: " << buffer << endl;
                if (strcmp(buffer, "Disconnect successful.") == 0) {
                    cout << ">> Now you are logged off and disconnected." << endl;
                    return true;
                }
            }
    return false;
}

/*
 * This function submits a message to the server.  It is used by other functions after they
 * generate the detailed contents of their particular messages.
 */
// Send message to server
bool Client::sendMessage(const string &title, char *message) const {
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

/*
 * This function parses incoming messages to tokens.
 */
void Client::parseTokens(char *buffer) {
    char *token;
    char *rest = (char *) buffer;

    while ((token = strtok_r(rest, ";", &rest))) {
        characterList.insert(token);
    }
}
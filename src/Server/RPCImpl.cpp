#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <vector>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "RPCImpl.h"
#include "LocalContext.h"


using namespace std;

typedef struct _GlobalContext {
    int g_rpcCount;
} GlobalContext;

GlobalContext globalObj; // We need to protect this, as we don't want bad data


RPCImpl::RPCImpl(int socket)
{
    socketID = socket;
    m_rpcCount = 0;
};

RPCImpl::~RPCImpl() {};


// Process all the RPCs
bool RPCImpl::rpcProcess() {
    char buffer[1024] = { 0 };
    vector<string> arrayTokens;
    ssize_t msgByte;
    bool continueOn = true;
    const int RPCTOKEN = 0;

    while ((continueOn)) {
        // Blocked until a RPC is sent to server
        msgByte = read(socketID, buffer, sizeof(buffer));

        cout << "Z0Z0Z0Z0Z0Z0 mybyte: " << msgByte << endl;
        if (msgByte <= 0) {
            printf("Read failed. Errno is %d.\n", errno);
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
    }
    return true;
}

// Parse incoming message
void RPCImpl::parseTokens(char* buffer, vector<string> &a) {
    char *token;
    char* rest = (char*)buffer;

    cout << ">> Parsing tokens." << endl;
    while ((token = strtok_r(rest, ";", &rest))) {
        a.emplace_back(token);
    }
}

// Print incoming message
void RPCImpl::printToken(vector<string> &arrayTokens) {
    // Enumerate through the tokens. The first token is always the specific RPC name
    cout << ">> Token(s) received: ";
    for (auto &arrayToken : arrayTokens) {
        printf("\n\ttoken = %s", arrayToken.c_str());
    }
    cout << endl;
}

// Send response to client
bool RPCImpl::sendResponse(char *message) const {
    size_t nlen = strlen(message);
    message[nlen] = 0;
    return send(socketID, message, strlen(message) + 1, 0);
}


// RPC: Connect/Login
bool RPCImpl::rpcConnect(vector<string> &arrayTokens) {
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
bool RPCImpl::rpcStatus() {
    cout << ">> Processing RPC: Status" << endl << endl;
    return true;
}

// RPC: Disconnect
bool RPCImpl::rpcDisconnect() {
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
bool RPCImpl::validLogin(const string &userName, const string &password) {
    string line, name, pwd;
    auto *users = new map<string, string>();

    // open file of approved users
    ifstream inFile("ApprovedUsers.txt");
    if (!inFile.is_open()) {
        cout << ">> Failed to read approved users list." << endl;
        delete users;
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
        delete users;
        return false;
    } else if (iter->second == password) {
        // found matching user name AND password
        string response = "User name and password validated.";
        cout << ">> " << response << endl << endl;
        sendResponse(&response[0]);
        delete users;
        return true;
    } else {
        // user name found, but password does not exist
        string response = "User name found, but password does not match.";
        cout << ">> " << response << endl << endl;
        sendResponse(&response[0]);
        delete users;
        return false;
    }
}


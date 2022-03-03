#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "RPCImpl.h"
#include <bits/stl_map.h>
#include "LocalContext.h"
#include "Game.h"


using namespace std;

typedef struct _GlobalContext {
    int g_rpcCount;
}
GlobalContext;  // WHat is this?
GlobalContext globalObj; // We need to protect this, as we don't want bad data




/*
 * RPC Implementation constructor
 */
RPCImpl::RPCImpl(int socket) {
    socketID = socket;
    m_rpcCount = 0; // TODO
    newGame = new Game;
};

/*
 * RPC Implementation destructor
 */
RPCImpl::~RPCImpl() {
    delete newGame;
}

/*
 * This function processes RPC received from a client, calling the appropriate function
 * once determined.
 */
bool RPCImpl::rpcProcess() {
    vector<string> arrayTokens;
    ssize_t msgByte;
    bool continueOn = true;
    const int RPCTOKEN = 0;

    while ((continueOn)) {
        // Buffer is re-initialized before processing each new RPC
        char buffer[1024] = {0};
        // Blocked until a RPC is sent to server
        msgByte = read(socketID, buffer, sizeof(buffer));
        if (msgByte <= 0) {
            printf("Read failed from Client %d. Errno is %d.\n", socketID, errno);
            break;
        }

        // Once an RPC is received
        printf(">> Client %d RPC received: %s\n", socketID, buffer);
        cout << ">> Total bytes: " << msgByte << endl;

        // Parse and Print the tokens
        arrayTokens.clear();
        parseTokens(buffer, arrayTokens);
        printToken(arrayTokens);

        // Get RPC name: string statements are not supported with a switch, so using if/else logic to dispatch
        string rpcName = arrayTokens[RPCTOKEN];

        if (rpcName == "connect") {  // This step is actually to authenticate user
            rpcConnect(arrayTokens);
        } else if (rpcName == "getCharacterList") {
            getCharacterList();
        } else if (rpcName == "disconnect") {
            rpcDisconnect();
            continueOn = false; // We are going to leave this loop, as we are done
        } else {
            string error = "Error: Invalid request";
            cerr << ">> " << error << endl;
            sendResponse(&error[0]);
            // Not in our list, perhaps, print out what was sent
        }
    }
    return true;
}

/*
 * This function parses incoming messages to identity the RPC and any
 * included arguments for the RPC.
 */
void RPCImpl::parseTokens(char *buffer, vector<string> &a) {
    char *token;
    char *rest = (char *) buffer;

    cout << ">> Parsing tokens." << endl;
    while ((token = strtok_r(rest, ";", &rest))) {
        a.emplace_back(token);
    }
}

/*
 * This function prints out the arguments included with an RPC.  This helps
 * with troubleshooting.
 */
void RPCImpl::printToken(vector<string> &arrayTokens) {
    // Enumerate through the tokens. The first token is always the specific RPC name
    cout << ">> Token(s) received: ";
    for (auto &arrayToken : arrayTokens) {
//        printf("\n\ttoken = %s", arrayToken.c_str());
        printf("%s  ", arrayToken.c_str());
    }
    cout << endl;
}

/*
 * This function sends a response to the client application via the established socket.
 */
bool RPCImpl::sendResponse(char *message) const {
    size_t nlen = strlen(message);
    message[nlen] = 0;
    return send(socketID, message, strlen(message) + 1, 0);
}

/*
 * This function validates a client's attempt to connect to the server.  Only permitted
 * clients are allowed to continue.
 */
bool RPCImpl::rpcConnect(vector<string> &arrayTokens) {
    cout << ">> Processing RPC: Connect" << endl << endl;
    printf(">> Validating login info for Client %d.\n", socketID);

    const int USERNAMETOKEN = 1;
    const int PASSWORDTOKEN = 2;

    // Strip out tokens 1 and 2 (username, password)
    string userNameString = arrayTokens[USERNAMETOKEN];
    string passwordString = arrayTokens[PASSWORDTOKEN];

    // Our Authentication Logic.
    return validLogin(userNameString, passwordString);

}

/*
 * This function reduces the source list to a character name list
 * and send it to the client to keep as a local copy.
 */
bool RPCImpl::getCharacterList() {
    cout << ">> Processing RPC: Generating character list." << endl << endl;
    cout << ">> Sending character list." << endl;

    // Generate character list and sent it to client
    string characterNames = getCharacterNamesOnly();
    if (!sendResponse(&characterNames[0])) {
        perror(">> Error: Failed to send character list to client.\n");
        return false;
    }
    cout << ">> Character list sent successfully.\n";
    return true;
}

string RPCImpl::getCharacterNamesOnly() {
    stringstream names;
    for (Character *person: newGame->getSourceList()) {
        names <<  person->getName();
        names << ";";
    }
    return names.str();
}

/*
 * This function disconnects the client from the server.
 */
bool RPCImpl::rpcDisconnect() {
    cout << ">> Processing RPC: Disconnect" << endl << endl;
    // Send Response back on our socket
    string response = "Disconnect successful.";
    if (!sendResponse(&response[0])) {
        perror(">> Error: Failed to send disconnect successful message.\n");
        return false;
    }
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


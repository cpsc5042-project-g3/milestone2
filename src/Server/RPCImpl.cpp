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
#include <algorithm>

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
}

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
        }
        else if (rpcName == "getCharacterList") {
            rpcGetCharacterList();
        }
        else if (rpcName == "getTraitList") {
            rpcGetTraitList();
        }
        else if (rpcName == "queryTrait") {
            rpcQueryTrait(arrayTokens);
        }
        else if (rpcName == "disconnect") {
            rpcDisconnect();
            continueOn = false; // We are going to leave this loop, as we are done
        }
        else {
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
bool RPCImpl::sendResponse(char* message) const {
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
bool RPCImpl::rpcGetCharacterList() {
    cout << ">> Processing RPC: Generating character list." << endl << endl;

    // Generate character list and sent to client.
    cout << ">> Sending character list." << endl;
    string characterNames = getCharacterNames();
    if (!sendResponse(&characterNames[0])) {
        perror(">> Error: Failed to send character list to client.\n");
        return false;
    }
    cout << ">> Character list sent successfully.\n";
    return true;
}

/*
 * This function returns the list of possible trait names.
 */
bool RPCImpl::rpcGetTraitList() {
    cout << ">> Processing RPC: Generating list of character traits." << endl << endl;

    // Generate character trait list and send to client.
    string characterTraits = getTraitNames();
    cout << ">> Sending list of traits to client." << endl;
    if (!sendResponse(&characterTraits[0])) {
        perror(">> Error: Failed to send list of traits to client.\n");
        return false;
    }
    cout << ">> Trait list sent successfully.\n";
    return true;
}

/*
 * This function parses the queryTrait RPC message from client.
 */
bool RPCImpl::rpcQueryTrait(vector<string> &arrayTokens) {
    cout << ">> Processing RPC: Query Trait" << endl << endl;

    const int TRAITNAME = 1;
    const int TRAITVALUE = 2;

    // Strip out tokens 1 and 2 (traitName, traitValue)
    string traitName = arrayTokens[TRAITNAME];
    string traitValue = arrayTokens[TRAITVALUE];

    // Get rid of hyphen from client input
    replace(traitName.begin(), traitName.end(), '-', ' ');

    return queryTraitResponse(traitName, traitValue);
}

/*
 * This function checks the supplied traitName and traitValue against
 * the selected game character. It returns true if the trait value matches.
 */
bool RPCImpl::queryTraitResponse(string& traitName, string& traitValue) {
    bool success;
    string message1;
    string expectedTraitValue;

    // compare query with game character trait value
    cout << ">> Comparing supplied trait name and value with game character." << endl;
    expectedTraitValue = newGame->getGameCharacter()->checkTraitValue(traitName);
    if (!expectedTraitValue.empty()) {
        success = expectedTraitValue == traitValue;
        if (!success) {
            cout << ">> Trait query DID NOT match game character traits." << endl;
            message1 = "Sorry.. " + customizedReply(traitName, traitValue, 2);
        }
        else {
            cout << ">> Trait query DID match game character traits." << endl;
            message1 = "Nice guess! " + customizedReply(traitName, traitValue, 1);
        }

        // notify client
        if (!sendResponse(&message1[0])) {
            perror(">> Error: Failed to send response to trait query.");
            return false;
        }
    }
    return true;
}

string RPCImpl::customizedReply(string& traitName, string &traitValue, int flag) {
    string message2;
    string expectedValue = newGame->getGameCharacter()->checkTraitValue(traitName);
    if (traitName == "Bald") {
        if (expectedValue == "Yes")
            message2 = "This person is bald.";
        else
            message2 = "This person is not bald.";
    }
    else if (traitName == "Gender") {
        if (expectedValue == "Male")
            message2 = "This person is male.";
        else
            message2 = "This person is female.";
    }
    else if (traitName == "Facial hair" || traitName == "Glasses" || traitName == "Hat") {
        if (expectedValue == "Yes")
            message2 = "This person does have " + traitName + ".";
        else
            message2 = "This person does not have " + traitName + ".";
    }
    else if (traitName == "Eye color" || traitName == "Hair color" || traitName == "Nose size") {
        if (flag == 1)
            message2 = "This person does have " + traitValue + " " + traitName + ".";
        else
            message2 = "This person does not have " + traitValue + " " + traitName + ".";
    }
    formatResponse(message2);
    return message2;

}

/*
 * This function returns a string containing the names of all the possible
 * characters a client can choose from.
 */
string RPCImpl::getCharacterNames() {
    stringstream names;
    for (Character *person: *newGame->getSourceList()) {
        names <<  person->getName();
        names << ";";
    }
    return names.str();
}

/*
 * This function returns the traits that can be guessed at by the client.
 */
string RPCImpl::getTraitNames() {
    return newGame->getGameCharacter()->getTraitNames();
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

/*
 * This function formats response to a word with the initial letter capitalized and the rest in lower case
 */
void RPCImpl::formatResponse(string &response) {
    transform(response.begin(), response.end(), response.begin(), [](unsigned char c) { return std::tolower(c); });
    response[0] = toupper(response[0]);
}
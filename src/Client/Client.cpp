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
#include <algorithm>
#include <regex>

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
    delete queryTraitName;
    delete queryTraitValue;
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
    string temp;
    char buffer[1024] = {0};

    // prompt for userName and password
    if (!getUserName() || !getPassword())
        return false;

    // Assemble login message to server
    ss << "connect;" << userName << ";" << password << ";";
    ss >> temp;
    int n = temp.length();
    char rpcMessage[n + 1];
    strcpy(rpcMessage, temp.c_str());

    // Send login message to server and get response
    if (sendMessage("RPC1: Login", rpcMessage)) {
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
 * This function submits a "RPC-getCharacterNames" to server and gets back a list
 * of character names, which will be parsed into a set of character names and
 * kept as a local copy.
 */
bool Client::getCharacterNamesFromServer() {
    char buffer[1024] = {0};
    string rpcMessage = "getCharacterList";
    char *getlistRPC = &rpcMessage[0];
    if (connected)
        if (sendMessage("RPC2: Get Character List", getlistRPC))
            // Get server response
            if (read(socketID, buffer, 1024)) {
                cout << ">> Character list received from server." << endl;
                parseTokens(buffer, "parseCharacter");
                cout << ">> Local copy made." << endl;
                return true;
            }
    perror(">> Error: Unable to get character list from server.");
    return false;
}

/*
 * This function submits a "RPC-getTraitList" to server and gets back a list
 * of trait names, which will be parsed into a set of trait names and
 * kept as a local copy.
 */
bool Client::getTraitListFromServer() {
    char buffer[1024] = {0};
    string temp = "getTraitList";
    char *getlistRPC = &temp[0];
    if (connected)
        if (sendMessage("RPC 3: Get Trait List", getlistRPC))
            // Get server response
            if (read(socketID, buffer, 1024)) {
                cout << ">> Trait list received from server." << endl;
                parseTokens(buffer, "parseTrait");
                cout << ">> Local copy made." << endl;
                return true;
            }
    perror(">> Error: Unable to get trait list from server.");
    return false;
}

/*
 * This function transmits a character query to the server
 */
bool Client::queryTrait() {
    stringstream ss;
    string temp;
    char buffer[1024] = {0};

    // prompt for trait name and value
    getQueryTraitName();
    getQueryTraitValue();

    // Assemble queryTrait message to server
    ss << "queryTrait;" << queryTraitName << ";" << queryTraitValue << ";";
    ss >> temp;
    int n = temp.length();
    char rpcMessage[n + 1];
    strcpy(rpcMessage, temp.c_str());

    if (connected)
        if (sendMessage("RPC4: Query trait", rpcMessage))

            // Get server response
            if (read(socketID, buffer, 1024)) {
                cout << ">> Server response: " << buffer << endl;
                return true;
            }
    perror(">> Error: Unable to send query trait RPC to server.");
    return false;
}

/*
 * This function prompts the user to enter their query trait name and verifies that
 * it is of an acceptable trait name.
 */
void Client::getQueryTraitName() {
    cin.clear();
    queryTraitName = new char[30];
    string answerPart1;  // answer to check
    string answerPart2;
    string answer; // answer to send
    bool valid = false;
    do {
        // Get console input
        cout << ">> Enter the name of the trait you want query about: ";
        cin >> answerPart1;
        trim(answerPart1);    // get rid of leading and trailing zeros
        formatAnswer(answerPart1);
        answer = answerPart1;
        // Expect a second console input for these trait names
        if (answer == "Eye" || answer == "Facial" || answer == "Hair" || answer == "Nose") {
            cin >> answerPart2;
            trim(answerPart2);
            answerPart1 += " " + answerPart2;  // Eg. "Eye color", add space before checking
            answer += "-" + answerPart2; // Eg. "Eye-color", add hyphen before send
            formatAnswer(answerPart1);
            formatAnswer(answer);
        }
        // Check if trait name is valid. If no, prompt user to enter again.
        if (traitList.find(answerPart1) != traitList.end()) {
            valid = true;
            strcpy(queryTraitName, answer.c_str());
        } else {
            cout << ">> Trait name not found." << endl;
        }
    } while (!valid);
}

/*
 * This function prompts the user to enter their query trait value.
 */
void Client::getQueryTraitValue() {
    cin.clear();
    queryTraitValue = new char[10];
    string answer;
    bool valid = false;

    string trait = queryTraitName;
    replace(trait.begin(), trait.end(), '-', ' ');
    if (trait == "Bald") {
        do {
            cout << "Do you think this person is " << trait << "? ";
            cin >> answer;
            trim(answer);
            valid = validateUserInput(answer, 1);
        } while (!valid);
    } else if (trait == "Gender") {
        do {
            cout << "Do you think this person is a male or female? ";
            cin >> answer;
            trim(answer);
            valid = validateUserInput(answer, 2);
        } while (!valid);
    } else if (trait == "Facial hair" || trait == "Glasses" || trait == "Hat") {
        do {
            cout << "Do you think this person has " << trait << "? ";
            cin >> answer;
            trim(answer);
            valid = validateUserInput(answer, 1);
        } while (!valid);
    } else if (trait == "Eye color" || trait == "Hair color") {
        cout << "What do you think is the person's " << trait << "? ";
        cin >> answer;
        trim(answer);
        strcpy(queryTraitValue, answer.c_str());
    } else if (trait == "Nose size") {
        do {
            cout << "Do you think this person has small, medium, or large nose? ";
            cin >> answer;
            trim(answer);
            valid = validateUserInput(answer, 3);
        } while (!valid);
    }
}

bool Client::validateUserInput(string &answer, int flag) {
    formatAnswer(answer);
    switch (flag) {
        case 1:
            if (answer == "Yes" || answer == "No") {
                strcpy(queryTraitValue, answer.c_str());
                return true;
            }
            cout << ">> Please enter yes or no." << endl;
            return false;
        case 2:
            if (answer == "Male" || answer == "Female") {
                strcpy(queryTraitValue, answer.c_str());
                return true;
            }
            cout << ">> Please enter male or female." << endl;
            return false;
        case 3:
            if (answer == "Small" || answer == "Medium" || answer == "Large") {
                strcpy(queryTraitValue, answer.c_str());
                return true;
            }
            cout << ">> Please enter small, medium, or large." << endl;
            return false;
    }
}


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
    char buffer[1024] = {0};
    // Assemble disconnect message
    string temp = "disconnect";
    char *logoffRPC = &temp[0];

    // Send disconnect message to server and get response
    if (connected)
        if (sendMessage("RPC 6: Disconnect", logoffRPC))
            // Get server response
            if (read(socketID, buffer, 1024)) {
                cout << ">> Server response: " << buffer << endl;
                if (strcmp(buffer, "Disconnect successful.") == 0) {
                    cout << ">> Now you are logged off and disconnected." << endl;
                    return true;
                }
            }
    perror(">> Error: Unable to disconnect from server.");
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
        cout << ">> Message sent successfully." << endl;
        return true;
    }
    cout << ">> " << title << " message failed to send." << endl;
    return false;
}

/*
 * This function parses incoming messages to tokens.
 */
void Client::parseTokens(char *buffer, string option) {
    char *token;
    char *rest = (char *) buffer;

    // Insert character names into a local list
    if (option == "parseCharacter")
        while ((token = strtok_r(rest, ";", &rest)))
            characterList.insert(token);

        // Insert trait names into a local list
    else if (option == "parseTrait")
        while ((token = strtok_r(rest, ";", &rest)))
            traitList.insert(token);
}

/*
 * This function formats answer to a word with the initial letter capitalized and the rest in lower case
 */
void Client::formatAnswer(string &answer) {
    transform(answer.begin(), answer.end(), answer.begin(), [](unsigned char c) { return std::tolower(c); });
    answer[0] = toupper(answer[0]);
}

/*
 * This function removes leading and trailing spaces
 */
string Client::trim(const string &s) {
    return ltrim(rtrim(s));
}

/*
 * This function removes leading spaces
 */
string Client::ltrim(const string &s) {
    return regex_replace(s, regex("^\\s+"), std::string(""));
}

/*
 * This function removes trailing spaces
 */
string Client::rtrim(const string &s) {
    return regex_replace(s, regex("\\s+$"), std::string(""));
}


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
#include <algorithm>
#include <regex>

using namespace std;
const string ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string ENDOFLINE = "\r\n";

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
    if (sendMessage("RPC 1: Login", rpcMessage)) {
        // Get server response
        if (read(socketID, buffer, 1024)) {
            cout << ">> Server response: " << buffer << endl;
            if (strcmp(buffer, "User name and password validated.") == 0) {
                printf(">> %s, you are now logged in.\n", userName);
                return true;
            }
            cout << endl; // please keep
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
    string rpcMessage = "getCharacterNames";
    char *getlistRPC = &rpcMessage[0];
    if (connected)
        if (sendMessage("RPC 2: Get Character Names", getlistRPC))
            // Get server response
            if (read(socketID, buffer, 1024)) {
                cout << ">> Character list received from server." << endl;
                makeLocalCopy(buffer, "parseCharacterNames");
                cout << ">> Local copy made." << endl;
                return true;
            }
    perror(">> Error: Unable to get character list from server.");
    return false;
}

/*
 * This function submits a "RPC-getTraitNames" to server and gets back a list
 * of trait names, which will be parsed into a set of trait names and
 * kept as a local copy.
 */
bool Client::getTraitNamesFromServer() {
    char buffer[1024] = {0};
    string temp = "getTraitNames";
    char *getTraitNameRPC = &temp[0];
    if (connected)
        if (sendMessage("RPC 3: Get Trait Names", getTraitNameRPC)) {
            // Get server response
            bool result = read(socketID, buffer, 1024);
            if (result) {
                cout << ">> Trait names received from server." << endl;
                makeLocalCopy(buffer, "parseTraitNames");
                cout << ">> Local copy made." << endl;
                return true;
            }
        }
    perror(">> Error: Unable to get trait names from server.");
    return false;
}

/*
 * This function submits a "RPC-getTraitValues" to server for each character and gets back all the trait values
 * associated with that character. The trait values will be mapped into a local active list, which contains all
 * the remaining characters and their associated traits.
 */
bool Client::getTraitValuesFromServer() {
    if (connected) {
        cout << "\n>> Sending RPC 4: Get Trait Values message to server." << endl;

        // Iterate through all characters to get their trait values
        for (const string &who: characterNames) {
            char buffer[1024] = {0};
            stringstream ss;
            string temp;

            // Assemble getTraitValues message, include character's name in message
            ss << "getTraitValues;" << who << ";";
            ss >> temp;
            unsigned long n = temp.length();
            char rpcMessage[n + 1];
            strcpy(rpcMessage, temp.c_str());

            // Send message to to server to get the trait values associated with the character
            if (!send(socketID, rpcMessage, strlen(rpcMessage) + 1, 0)) {
                printf(">> RPC 4: Get Trait Values message failed to send for character %s.\n", who.c_str());
                return false;
            }

            // Get server response
            if (!read(socketID, buffer, 1024)) {
                printf(">> Error: Unable to get trait values from server for character %s.\n", who.c_str());
                return false;
            }

            // Add trait value info to map of active list
            makeLocalCopy(buffer, "parseTraitValues");
        }
        cout << ">> Message sent successfully." << endl;
        cout << ">> Trait values received for all characters from server." << endl;
        cout << ">> Local copy made." << endl << endl;
        return true;
    }

}

/*
 * This function transmits a character query to the server
 */
bool Client::queryTrait() {
    stringstream ss;
    string temp;
    char buffer[1024] = {0};

    // prompt for trait name and value
    if (!getQueryTraitName())
        return false;
    getQueryTraitValue();

    // Assemble queryTrait message to server
    ss << "queryTrait;" << queryTraitName << ";" << queryTraitValue << ";";
    ss >> temp;
    int n = temp.length();
    char rpcMessage[n + 1];
    strcpy(rpcMessage, temp.c_str());

    if (connected)
        if (sendMessage("RPC 5: Query trait", rpcMessage))

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
bool Client::getQueryTraitName() {
    cin.clear();
    queryTraitName = new char[30];
    string answerPart1;  // answer to check
    string answerPart2;
    string answer; // answer to send
    bool valid = false;
    do {
        // Get console input
        cout << ">> Enter the name of the trait you want query about (or 'x' to return to previous menu): ";
        cin >> answerPart1;
        if (answerPart1 == "x")
            return false;
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
        if (traitNames.find(answerPart1) != traitNames.end()) {
            valid = true;
            strcpy(queryTraitName, answer.c_str());
        } else {
            cout << ">> Trait name not found." << endl;
        }
    } while (!valid);
    return true;
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
        validateUserInput(answer, 4);
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
        default:
            return true;
    }
}

/*
 * This function updates the active list and the character names list based on users' elimination choices.
 */
bool Client::eliminatePerson() {
    vector<int> rowNumbers;
    string eliminatedName;

    // If user picked 'x' to return to previous menu
    if (!getEliminateChoice(rowNumbers))
        return false;

    // Otherwise, continue on to eliminate
    string temp[characterNames.size()]; // temp array to hold the characters IN ORDER
    int vectorSizeShrink = 1;

    // Copy the remaining character names into a temporary array to match table row number
    for (int i = 0; i < characterNames.size(); i++)
        temp[i] = characterNames.at(i);

    cout << "\nThese characters have been eliminated from the table: ";

    for (int rowNumber:rowNumbers) {
        eliminatedName = temp[rowNumber - 1];
        // Update the active list
        activeList.erase(eliminatedName);

        // Update the character names list
        characterNames.erase(characterNames.begin() + rowNumber - vectorSizeShrink);
        cout << eliminatedName << "  ";
        vectorSizeShrink++;
    }
    cout << endl << endl;
}

/*
 * This function asks user whom they want to eliminate from the active list. Use input is validated.
 */
bool Client::getEliminateChoice(vector<int> &rowNumbers) {
    string count;
    string rowChoice;
    int size = characterNames.size();
    bool valid;

    do {
        cout << "How many characters do you want to eliminate? (Enter x if you want to return to previous menu) ";
        cin >> count;
        if (count == "x")
            return false;
        valid = validateNumericInput(count, size);
    } while (!valid);

    do {
        valid = false;
        rowNumbers.clear();
        printf("You want to eliminate %d characters.\n", stoi(count));
        for (int i = 0; i < stoi(count); i++) {
            cout << "Enter the ROW number of character No. " << i + 1 << ": ";
            cin >> rowChoice;
            valid = validateNumericInput(count, size);
            if (!valid)
                break;
            rowNumbers.emplace_back(stoi(rowChoice));
        }
        sort(rowNumbers.begin(), rowNumbers.end());
    } while (!valid);
    return true;
}

/*
 * This function submits a guess to the server about the target characters name.
 */
bool Client::guessName() {
    stringstream ss;
    string temp;
    char buffer[1024] = {0};

    // Prompt user to enter guess
    string guess = getUserGuess();

    // If user entered x, return to previous menu
    if (guess == "x")
        return false;

    // Otherwise, assemble login message to server
    ss << "finalGuess;" << guess << ";";
    ss >> temp;
    int n = temp.length();
    char rpcMessage[n + 1];
    strcpy(rpcMessage, temp.c_str());

    // Send final guess message to server and get response
    if (sendMessage("RPC 8: Final Guess", rpcMessage)) {
        // Get server response
        if (read(socketID, buffer, 1024)) {
            cout << ">> Server response: ";
            if (strcmp(buffer, "Correct") == 0) {
                printf("Congratulations %s! you have made the right guess!\n", userName);
                return true;
            }
        }
    }
    printf("Sorry %s, you did not make the right guess. The character is: %s.\n", userName, buffer);
    return false;
}

/*
 * This function prompts the user to enter their guess.
 */
string Client::getUserGuess() {
    string guess;
    cout << ">> Who do you think the character is? Enter the name (or 'x' to return to previous menu): ";
    cin >> guess;
    if (guess == "x")
        return guess;
    cout << ">> You've entered " << guess << ", now let's check if you are right!" << endl;
    formatAnswer(guess); // accept upper or lower case answer
    return guess;
}

/*
 * This function sends a request to get leader board from server.
 */
bool Client::getLeaderBoard() {
    char buffer[1024] = {0};
    string temp = "getLeaderBoard";
    char *getLeaderBoardRPC = &temp[0];
    if (connected)
        if (sendMessage("RPC 7: Get leader board", getLeaderBoardRPC)) {
            // Get server response
            bool result = read(socketID, buffer, 1024);
            if (result) {
                cout << ">> Leader board received from server." << endl;
                if (strcmp(buffer, "Empty") == 0)
                    return false;
                makeLocalCopy(buffer, "parseLeaderBoard");
                cout << ">> Local copy made." << endl;
                return true;
            }
        }
    perror(">> Error: Unable to get leader board from server.");
    return false;

}

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
 * This function parses a buffer message from server into tokens and store the information in a local copy.
 */
void Client::makeLocalCopy(char *buffer, string option) {
    char *token;
    char *rest;
    string str(buffer);

    // trim any end of line constants
    int first = str.find_first_of(ALPHABET);
    int last = str.find_first_of(ENDOFLINE);
    string trimmed = str.substr(first, last - first);
    rest = &trimmed[0];

    // Insert character names into a local list
    if (option == "parseCharacterNames") {
        while ((token = strtok_r(rest, ";", &rest)))
            characterNames.emplace_back(token);
    }

        // Insert trait names into 2 local lists
    else if (option == "parseTraitNames") {
        while ((token = strtok_r(rest, ";", &rest))) {
            traitNames.insert(token);
            traitNamesForDisplay.emplace_back(token);
        }
    }

        // Insert trait values into a map of active list
    else if (option == "parseTraitValues") {
        vector<string> currTraits;
        while ((token = strtok_r(rest, ";", &rest)))
            currTraits.emplace_back(token);
        string who = currTraits.at(0);
        if (activeList.find(who) == activeList.end())
            activeList.insert(make_pair(who, currTraits));
    }

        // Insert leader names and leader scores into local copy
    else if (option == "parseLeaderBoard") {
        leaderBoard.clear();
        while ((token = strtok_r(rest, ";", &rest)))
            leaderBoard.emplace_back(token);
    }
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

/*
 * This function validates that user's input only contains digit and is a valid row number or count.
 */
bool Client::validateNumericInput(const string &str, int size) {
    if (str.find_first_not_of("0123456789") != string::npos || stoi(str) > size || stoi(str) < 1) {
        printf(">> Error: Please enter a number between 1 and %d.\n", size);
        return false;
    }
    return true;
}
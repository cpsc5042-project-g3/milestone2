#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "RPCImpl.h"
#include <bits/stl_map.h>
#include "LocalContext.h"
#include "Game.h"
#include <algorithm>
#include <semaphore.h>
#include <iomanip>

using namespace std;

const int LEADERBOARD_SIZE = 5;
const int NIGHT_NIGHT = 1;
const string ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string ENDOFLINE = "\r\n";
vector<string> leaderNames;     // global variable, protected by semaphore
vector<int> leaderScores;       // global variable, protected by semaphore; holds the queryCounts of each player
int minScore = 1000000;         // global variable, protected by semaphore
sem_t sem;
pthread_mutex_t mutex;



/*
 * RPC Implementation constructor
 */
RPCImpl::RPCImpl(int socket) {
    socketID = socket;
    userName = "";
    newGame = new Game;
    queryCount = 0;
    traitSent = 0;
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
    string temp;

    while ((continueOn)) {
        // Buffer is re-initialized before processing each new RPC
        char buffer[1024] = {0};

        // Blocked until a RPC is sent to server
        msgByte = read(socketID, buffer, sizeof(buffer));
        if (msgByte <= 0) {
            cerr << ">> Error: Read failed from Client " << socketID - 3 << ".";
            break;
        }

        temp = buffer;

        // Parse the tokens
        arrayTokens.clear();
        parseTokens(buffer, arrayTokens);

        // Get RPC name: string statements are not supported with a switch, so using if/else logic to dispatch
        string rpcName = arrayTokens[RPCTOKEN];

        if (rpcName == "connect") {  // This step is actually to authenticate user
            userName = arrayTokens[1];
            printf("\n>> RPC 1 received from %s (bytes = %d): %s\n", userName.c_str(), msgByte, temp.c_str());
            rpcConnect(arrayTokens);
        } else if (rpcName == "getCharacterNames") {
            printf("\n>> RPC 2 received from %s (bytes = %d): %s\n", userName.c_str(), msgByte, temp.c_str());
            rpcGetCharacterNames();
        } else if (rpcName == "getTraitNames") {
            printf(">> RPC 3 received from %s (bytes = %d): %s\n", userName.c_str(), msgByte, temp.c_str());
            rpcGetTraitNames();
        } else if (rpcName == "getTraitValues") {
            traitSent++;
            // Since there are many characters, instead of printing Server console output for each character
            // Only print ">> Client RPC received" message once upon receiving the first character's request
            if (traitSent == 1) {
                printf(">> RPC 4 received from %s (bytes = %d): getTraitValues\n", userName.c_str(), msgByte);
                printf(">> Processing RPC: Generating a list of trait values for each character.\n");
            }

            rpcGetTraitValues(arrayTokens);

            // Only print ">> Trait values sent successfully message." once upon sending the last character's request
            if (traitSent == newGame->characterNames.size()) {
                cout << ">> Trait values sent successfully for all characters.\n\n";
                traitSent = 0;
            }
        } else if (rpcName == "queryTrait") {
            printf(">> RPC 5 received from %s (bytes = %d): %s\n", userName.c_str(), msgByte, temp.c_str());
            rpcQueryTrait(arrayTokens);
        } else if (rpcName == "finalGuess") {
            printf(">> RPC 6 received from %s (bytes = %d): %s\n", userName.c_str(), msgByte, temp.c_str());
            rpcFinalGuess(arrayTokens);
        } else if (rpcName == "getLeaderBoard") {
            printf(">> RPC 7 received from %s (bytes = %d): %s\n", userName.c_str(), msgByte, temp.c_str());
            rpcGetLeaderBoard();
        } else if (rpcName == "disconnect") {
            printf(">> RPC 8 received from %s (bytes = %d): %s\n", userName.c_str(), msgByte, temp.c_str());
            rpcDisconnect();
            continueOn = false; // We are going to leave this loop, as we are done
        } else {
            string error = "Error: Invalid request";
            cerr << ">> " << error << endl;
            sendResponse(&error[0]);
            // Not in our list
        }
    }
    return true;
}

/*
 * This function validates a client's attempt to connect to the server.  Only permitted
 * clients are allowed to continue.
 */
bool RPCImpl::rpcConnect(vector<string> &arrayTokens) {
    cout << ">> Processing RPC: Connect" << endl;

    // sleep to allow for demonstration of simultaneous activity
    sleep(NIGHT_NIGHT);

    const int USERNAMETOKEN = 1;
    const int PASSWORDTOKEN = 2;

    // Strip out tokens 1 and 2 (username, password)
    string userNameString = arrayTokens[USERNAMETOKEN];
    string passwordString = arrayTokens[PASSWORDTOKEN];

    // Our Authentication Logic.
    printf(">> Validating login info for %s.\n", userName.c_str());
    return validLogin(userNameString, passwordString);
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
        cerr << ">> Error: Failed to read approved users list." << endl;
        delete users;
        return false;
    }

    // read approved users and their passwords line by line
    while (inFile.peek() != EOF) {
        getline(inFile, line);

        // trim line
        int first = line.find_first_of(ALPHABET);
        int last = line.find_first_of(ENDOFLINE);
        line = line.substr(first, last - first);

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
        if (!sendResponse(&response[0])) {
            perror(">> Error: Failed to send invalid user name message to client.\n");
        }
        delete users;
        return false;
    } else if (iter->second == password) {
        // found matching user name AND password
        userID = userName;
        string response = "User name and password validated.";
        cout << ">> " << response << endl;
        if (!sendResponse(&response[0])) {
            perror(">> Error: Failed to send login validation message to client.\n");
        }
        delete users;
        return true;
    } else {
        // user name found, but password does not exist
        string response = "User name found, but password does not match.";
        cout << ">> " << response << endl << endl;
        if (!sendResponse(&response[0])) {
            perror(">> Error: Failed to send password not match message to client.\n");
        }
        delete users;
        return false;
    }
}

/*
 * This function reduces the source list to a character name list
 * and send it to the client to keep as a local copy.
 */
bool RPCImpl::rpcGetCharacterNames() {
    cout << ">> Processing RPC: Generating character names." << endl;

    // sleep to allow for demonstration of simultaneous activity
    sleep(NIGHT_NIGHT);

    // Generate character list and sent to client.
    cout << ">> Sending character names." << endl;
    string characterNames = getCharacterNames();
    if (!sendResponse(&characterNames[0])) {
        perror(">> Error: Failed to send character names to client.\n");
        return false;
    }
    cout << ">> Character names sent successfully.\n\n";
    return true;
}

/*
 * This function returns a string containing the names of all the possible
 * characters a client can choose from.
 */
string RPCImpl::getCharacterNames() {
    stringstream names;
    for (const string &aName:newGame->characterNames)
        names << aName << ";";
    return names.str();
}

/*
 * This function returns the list of possible trait names.
 */
bool RPCImpl::rpcGetTraitNames() {
    cout << ">> Processing RPC: Generating list of trait names." << endl;

    // sleep to allow for demonstration of simultaneous activity
    sleep(NIGHT_NIGHT);

    // Generate character trait name list and send to client.
    string characterTraits = newGame->traitNamesForDisplay;
    cout << ">> Sending list of trait names to client." << endl;
    if (!sendResponse(&characterTraits[0])) {
        perror(">> Error: Failed to send trait names to client.\n");
        return false;
    }
    cout << ">> Trait names sent successfully.\n\n";
    return true;
}

/*
 * This function returns a list of trait values associated with the character.
 */
bool RPCImpl::rpcGetTraitValues(vector<string> &arrayTokens) {
    // sleep to allow for demonstration of simultaneous activity
    // TODO uncomment sleep prior to submitted code to Canvas
    //sleep(1);

    const int CHARACTER_NAME = 1;
    string currCharacter = arrayTokens[CHARACTER_NAME];

    // Get the list of trait values to send to client, include character name at the beginning
    string traitInfo = currCharacter + ";";
    traitInfo += newGame->sourceList->find(currCharacter)->second->traitValuesForDisplay;

    printf("\tSending trait values to %s: %s\n", userName.c_str(), traitInfo.c_str());
    if (!sendResponse(&traitInfo[0])) {
        printf(">> Error: Failed to send %s's trait values to client.\n", currCharacter.c_str());
        return false;
    }
    return true;
}

/*
 * This function parses the queryTrait RPC message from client.
 */
bool RPCImpl::rpcQueryTrait(vector<string> &arrayTokens) {
    cout << ">> Processing RPC: Query Trait" << endl;

    // sleep to allow for demonstration of simultaneous activity
    sleep(NIGHT_NIGHT);

    const int TRAITNAME = 1;
    const int TRAITVALUE = 2;

    // Strip out tokens 1 and 2 (traitName, traitValue)
    string traitName = arrayTokens[TRAITNAME];
    string traitValue = arrayTokens[TRAITVALUE];

    // Get rid of hyphen from client input
    replace(traitName.begin(), traitName.end(), '-', ' ');

    // keep score
    queryCount++;

    return queryTraitResponse(traitName, traitValue);
}

/*
 * This function checks the supplied traitName and traitValue against
 * the selected game character. It returns true if the trait value matches.
 */
bool RPCImpl::queryTraitResponse(string &traitName, string &traitValue) {
    bool success;
    string message1;
    string expectedTraitValue;

    // compare query with game character trait value
    cout << ">> Comparing supplied trait name and value with game character." << endl;
    expectedTraitValue = newGame->getGameCharacter()->checkTraitValue(traitName);
    if (!expectedTraitValue.empty()) {
        success = expectedTraitValue == traitValue;
        if (!success) {
            cout << ">> Trait query DID NOT match game character traits.\n" << endl;
            message1 = "Sorry. " + customizedReply(traitName, traitValue, 2);
        } else {
            cout << ">> Trait query DID match game character traits.\n" << endl;
            message1 = "Nice guess! " + customizedReply(traitName, traitValue, 1);
        }

        // notify client
        if (!sendResponse(&message1[0])) {
            perror(">> Error: Failed to send response to trait query.\n");
            return false;
        }
    }
    return true;
}

/*
 *
 */
string RPCImpl::customizedReply(string &traitName, string &traitValue, int flag) {
    string message2;
    string expectedValue = newGame->getGameCharacter()->checkTraitValue(traitName);
    if (traitName == "Bald") {
        if (expectedValue == "Yes")
            message2 = "This person is bald.";
        else
            message2 = "This person is not bald.";
    } else if (traitName == "Gender") {
        if (expectedValue == "Male")
            message2 = "This person is male.";
        else
            message2 = "This person is female.";
    } else if (traitName == "Facial hair" || traitName == "Glasses" || traitName == "Hat") {
        if (expectedValue == "Yes")
            message2 = "This person does have " + traitName + ".";
        else
            message2 = "This person does not have " + traitName + ".";
    } else if (traitName == "Eye color" || traitName == "Hair color" || traitName == "Nose size") {
        if (flag == 1)
            message2 = "This person does have " + traitValue + " " + traitName + ".";
        else
            message2 = "This person does not have " + traitValue + " " + traitName + ".";
    }
    formatResponse(message2);
    return message2;

}

/*
 * This function formats response to a word with the initial letter capitalized and the rest in lower case
 */
void RPCImpl::formatResponse(string &response) {
    transform(response.begin(), response.end(), response.begin(), [](unsigned char c) { return std::tolower(c); });
    response[0] = toupper(response[0]);
}

/*
 * This function processes the final guess and checks if user has won.
 */
bool RPCImpl::rpcFinalGuess(vector<string> &arrayTokens) {
    cout << ">> Processing RPC: Final Guess" << endl;

    // sleep to allow for demonstration of simultaneous activity
    sleep(NIGHT_NIGHT);

    // Get user final guess
    const int FINAL_GUESS = 1;
    string traitName = arrayTokens[FINAL_GUESS];
    string resultMsg;
    string expectedName = newGame->getGameCharacter()->getName();

    // Send guess result to client
    cout << ">> Checking if guess is correct." << endl;
    if (traitName == expectedName) {
        resultMsg = "Correct";  // do not change this string
        cout << ">> User guessed correctly." << endl;
        updateLeaderboard(queryCount, userID);
    }
    else {
        resultMsg = expectedName; // if incorrect, send correct answer to client
        cout << ">> User did not make the right guess." << endl;
    }
    if (!sendResponse(&resultMsg[0])) {
        perror(">> Error: Failed to send final result to client.\n");
        return false;
    }
    cout << ">> Result sent to client successfully.\n\n";
    return true;
}

/*
 * This function sends the leader board info to the client.
 */
bool RPCImpl::rpcGetLeaderBoard() {
    cout << ">> Processing RPC: Get leader board" << endl;

    // sleep to allow for demonstration of simultaneous activity
    sleep(NIGHT_NIGHT);

    string response;

    // If leader board is empty, send "Empty"
    if (leaderNames.empty())
        response = "Empty";

    // Else, assemble leader border info, separate everything by ";"
    else {
        stringstream ss;
        for (int i = 0; i < leaderNames.size(); i++)
            ss << leaderNames[i] << ";" << leaderScores[i] << ";";
        response = ss.str();
    }
    // Send leader board to client
    if (!sendResponse(&response[0])) {
        perror(">> Error: Failed to send leader board to client.\n");
        return false;
    }
    cout << ">> Leader board sent successfully." << endl;
    return true;
}

/*
 * This function disconnects the client from the server.
 */
bool RPCImpl::rpcDisconnect() {
    cout << ">> Processing RPC: Disconnect" << endl << endl;

    // sleep to allow for demonstration of simultaneous activity
    sleep(NIGHT_NIGHT);

    // Send Response back on our socket
    string response = "Disconnect successful.";
    if (!sendResponse(&response[0])) {
        perror(">> Error: Failed to send disconnect successful message.\n");
        return false;
    }
    return true;
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
 * This function parses incoming messages to identity the RPC and any
 * included arguments for the RPC.
 */
void RPCImpl::parseTokens(char *buffer, vector<string> &a) {
    char *token;
    char *rest = (char *) buffer;
    while ((token = strtok_r(rest, ";", &rest)))
        a.emplace_back(token);
}

/*
 * This function gets the maximum queryCount from all the players.
 * A max queryCount will lead to a min total score
 */
int getMaxQueryCount() {
    int queryCount = 0;

    for (int count : leaderScores) {
        // traverse vector to find max query count
        if (count > queryCount)
            queryCount = count;
    }
    return queryCount;
}

/*
 * This function prints leader board on Server side. Print order is based on score in descending order.
 */
void RPCImpl::printLeaderboard() {
    // print header
    cout << "Current Leaderboard" << endl;
    cout << "Name        Queries" << endl;
    cout << "-------------------" << endl;

    // print players in leaderboard
    for (int i = 0; i < leaderScores.size(); i++) {
        cout << setw(12) << left << leaderNames[i] << setw(12) << left << leaderScores[i] << endl;
    }
    cout << endl;
}

/*
 * This function updates the leader boarder. Leader border is sorted based on score in descending order.
 */
void RPCImpl::updateLeaderboard(int score, const string &name) {
    // wait until semaphore says go
    cout << ">> Updating leaderboard for " << userName << "." << endl << endl;

    sem_wait(&sem);
    pthread_mutex_lock(&mutex);

    // Add score
    if (leaderScores.size() < LEADERBOARD_SIZE) {
        leaderScores.emplace_back(score);
        leaderNames.emplace_back(name);
        minScore = getMaxQueryCount();
    }

    // If leaderboard is full, check if score qualifies and replace lowest score
    // if score == minScore, replace the last player to keep most recent player
    else if (score <= minScore) {
        for (int i = leaderScores.size(); i-- > 0;) {
            if (leaderScores[i] == minScore) {
                leaderScores[i] = score;
                leaderNames[i] = name;
                minScore = getMaxQueryCount();
                i = 0;      // break out of loop
            }
        }
    }

    // Sort leaderboard
    selectionSort();

    // print out leaderboard
    printLeaderboard();

    // end of critical section, allow others to use semaphore
    pthread_mutex_unlock(&mutex);
    sem_post(&sem);
}

/*
 * This function sorts the leader scores in ascending order.
 * NOTE: "leader score" here is based on queryCount.
 * The order of leader names will be also updated in accordance.
 */
void RPCImpl::selectionSort() {
    int min;
    int n = leaderScores.size();
    for (int i = 0; i < n; i++) {
        min = i;
        for (int j = i + 1; j < n; j++)
            if (leaderScores[j] < leaderScores[min])
                min = j;
        swapScores(&leaderScores[min], &leaderScores[i]);
        swapNames(&leaderNames[min], &leaderNames[i]);
    }
}

/*
 * This function swaps the leader scores.
 */
void RPCImpl::swapScores(int *i, int *j) {
    int temp = *i;
    *i = *j;
    *j = temp;
}

/*
 * This function swaps the leader names.
 */
void RPCImpl::swapNames(string *i, string *j) {
    string temp = *i;
    *i = *j;
    *j = temp;
}


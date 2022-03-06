/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 10 Mar 2022
 */

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <cstring>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using namespace std;

class Client {
public:
    char* userName;                                   // player's login name
    vector<string> characterNames;                    // a local list of character names
    unordered_set<string> traitNames;                 // a local list of trait names for searching
    vector<string> traitNamesForDisplay;              // a local list of trait names for display
    unordered_map<string, vector<string>> activeList; // a map of remaining characters and their traits

    Client();
    ~Client();
    bool connectServer(const char* serverIP, int port); // RPC 1
    bool logIn();
    bool getCharacterNamesFromServer(); // RPC 2
    bool getTraitNamesFromServer(); // RPC 3
    bool getTraitValuesFromServer(); // RPC 4
    bool queryTrait(); // RPC 5
    bool eliminatePerson();
    bool guessName(); // RPC 6
    bool disconnectServer(); // TODO

private:
    const int MAX_LEN = 10;         // player's login name length limit
    char* password;                 // player's login password
    char* queryTraitName;           // name of trait player wants to query
    char* queryTraitValue;          // value of trait player wants to query
    int socketID;
    bool connected;

    bool sendMessage(const string& title, char* message) const;
    bool getUserName();
    bool getPassword();
    void getQueryTraitName();
    void getQueryTraitValue();
    bool validateUserInput(string &answer, int flag);
    void getEliminateChoice(vector<int> &rowNumbers);
    void makeLocalCopy(char *buffer, string option);
    static void formatAnswer(string &answer);
    static string getUserGuess();
    string trim(const string &s);
    string ltrim(const string &s);
    string rtrim(const string &s);
    bool validateNumericInput(const string &str, int size);

};

#endif //CLIENT_CLIENT_H

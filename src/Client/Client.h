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
#include <set>

using namespace std;

class Client {
public:
    char* userName;
    set<string> characterList;
    set<string> traitList;
    vector<vector<string>> activeList;

    Client();
    ~Client();
    bool connectServer(const char* serverIP, int port);
    bool logIn();
    bool disconnectServer();
    bool getCharacterNamesFromServer();
    bool getTraitListFromServer();
    bool queryTrait();
    bool guessName(const char* name);
    bool eliminatePerson(const char* name);

private:
    const int MAX_LEN = 10;
    char* password;
    char* queryTraitName;
    char* queryTraitValue;
    int socketID;
    bool connected;

    bool sendMessage(const string& title, char* message) const;
    bool getUserName();
    bool getPassword();
    void getQueryTraitName();
    void getQueryTraitValue();
    bool validateUserInput(string &answer, int flag);
    void parseTokens(char *buffer, string option);
    void formatAnswer(string &answer);
    string trim(const string &s);
    string ltrim(const string &s);
    string rtrim(const string &s);

};

#endif //CLIENT_CLIENT_H

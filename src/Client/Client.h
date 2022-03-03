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

    Client();
    ~Client();
    bool connectServer(const char* serverIP, int port);
    bool logIn();
    bool disconnectServer();
    bool getCharacterNamesFromServer();
    bool queryTrait(const char* trait, const char* traitValue);
    bool guessName(const char* name);
    bool eliminatePerson(const char* name);

private:
    const int MAX_LEN = 10;

    char* password;
    int socketID;
    bool connected;

    bool sendMessage(const string& title, char* message) const;
    bool getUserName();
    bool getPassword();
    void parseTokens(char *buffer);

};

#endif //CLIENT_CLIENT_H

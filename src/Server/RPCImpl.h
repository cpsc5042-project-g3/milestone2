/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 12 Mar 2022
 */

#pragma once
#include "Game.h"
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <iterator>

using namespace std;
class RPCImpl {
public:
    explicit RPCImpl(int socket);
    ~RPCImpl();
    bool rpcProcess();
    void printLeaderboard();
    void updateLeaderboard(int score, const string &name);
    string getCharacterNames();

private:
    int socketID;       // connection socket descriptor for each client
    string userName;    // client login name
    string userID;      // unique client ID in case of duplicate login name
    int queryCount;     // number of queries client asked
    Game *newGame;      // new game object
    int traitSent;      // keeps track of how many trait values have been sent

    bool rpcConnect(vector<std::string>& arrayTokens);
    bool validLogin(const string& userName, const string& password);
    bool rpcGetCharacterNames();
    bool rpcGetTraitNames();
    bool rpcGetTraitValues(vector<string> &arrayTokens);
    bool rpcQueryTrait(vector<string> &arrayTokens);
    bool queryTraitResponse(string& traitName, string& traitValue);
    string customizedReply(string& traitName, string &traitValue, int flag);
    static void formatResponse(string &response);
    bool rpcFinalGuess(vector<string> &arrayTokens);
    bool rpcGetLeaderBoard();
    bool rpcDisconnect();
    bool sendResponse(char* message) const;
    static void parseTokens(char* buffer, vector<string>& a);
    void selectionSort();
    void swapScores(int *i, int *j);
    void swapNames(string *i, string *j);

};


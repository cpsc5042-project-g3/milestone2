/* RPCServer.cpp : This file contains the 'main' function.Program execution begins and ends there.
 This is a very very simple example of a CPSC5042 Server that will listen to
 a CPSC5042 Client
 Version 1 will have the server handle one client at a time. The server will:
 - Wait for connection from client
 - Process the Connect API  once connect
 - Process all RPC requests until the client does a Disconnect RPC
 - This intial server will handle 3 RPC's:
 -      Connect
 -      HowManyChars (will return HowManyChars are in the input message)
 -      Disconnnect
 server, then run the various RPC's that might happen between the server and
 client
*/

#pragma once
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>
#include <vector>
#include <iterator>
#include "Game.h"

using namespace std;
class RPCImpl {
public:
    explicit RPCImpl(int socket);
    ~RPCImpl();
    bool rpcProcess();
    void updateLeaderboard(int score, const string &name);
    string getCharacterNames();

private:
    const int BUFFER_SIZE = 1024;
    int m_rpcCount;
    int socketID;
    int queryCount;
    string userID;
    Game *newGame;

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
    bool rpcDisconnect();
    bool sendResponse(char* message) const;
    static void parseTokens(char* buffer, vector<string>& a);
    static void printToken(vector<string>& a);

};


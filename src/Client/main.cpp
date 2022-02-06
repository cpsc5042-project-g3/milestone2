/*
 * Client side C/C++ program to demonstrate Socket programming.  This is a C Program.
 * No classes. You may turn this into an Object Oriented C++ program if you wish.
*/

#include <cstdio>
#include <cstring>
#include <vector>
#include <iterator>
#include "Client.h"

using namespace std;

void ParseTokens(char* buffer, std::vector<std::string>& a)
{
    char* token;
    char* rest = (char*)buffer;

    while ((token = strtok_r(rest, ";", &rest)))
    {
        printf("%s\n", token);
        a.emplace_back(token);
    }
}

int main(int argc, char const* argv[])
{
    char* userName = (char*) "MIKE";
    char* password = (char*) "MIKE";
    const char* serverIP = argv[1];
    const int port = atoi(argv[2]);
    bool connected;

    auto* client = new Client();
    client->setUserName(userName);
    client->setPassword(password);

    // Connect to server
    connected = client->ConnectServer(serverIP, port);
    if (connected) {}

    // Disconnect from server
    connected = client->DisconnectServer();
    if (connected) {}

    // Cleanup memory
    delete client;
    return 0;
}
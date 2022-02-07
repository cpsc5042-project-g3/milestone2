/*
 * Client side C/C++ program to demonstrate Socket programming.  This is a C Program.
 * No classes. You may turn this into an Object Oriented C++ program if you wish.
*/

#include <cstdio>
#include <cstring>
#include <vector>
#include <iterator>
#include <iostream>
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

void getUser(char *& userName) {
    cin.clear();
    cout << "Enter valid username: ";
    cin >> userName;
    if (cin.fail()) {
        cin.clear();
        cout << "Bad input." << endl;
        userName = (char*) " ";
    }
}

void getPassword(char *& password) {
    cin.clear();
    cout << "Enter the associated password: ";
    cin >> password;
    if (cin.fail()) {
        cin.clear();
        cout << "Bad input." << endl;
        password = (char*) " ";
    }
}

int main(int argc, char const* argv[])
{
    const int MAX_LEN = 10;
    char* userName = new char[MAX_LEN];
    char* password = new char[MAX_LEN];
    getUser(userName);
    getPassword(password);
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
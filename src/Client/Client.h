

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <cstring>

class Client {
public:
    Client();
    ~Client();
    bool ConnectServer(const char* serverIP, int port);
    bool queryTrait(const char* trait, const char* traitValue);
    bool guessName(const char* name);
    bool eliminatePerson(const char* name);
    bool DisconnectServer() const;
    bool setUserName(char* userNameIn);
    bool setPassword(char* passwordIn);

private:
    char* userName;
    char* password;
    char* buffer;
    int socketID;
    bool connected;
};

#endif //CLIENT_CLIENT_H

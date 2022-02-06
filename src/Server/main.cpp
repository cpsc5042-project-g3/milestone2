#include <cstdio>
#include <iostream>
#include "Server.h"

using namespace std;

int main(int argc, char const* argv[])
{
    //
    const char* serverIP = argv[1];
    int port = atoi(argv[2]);

    bool statusOk = true;
    Server* serverObj = new Server(serverIP, port);

#if 0
    std::vector<std::string> tokArray;
    char buffer[128] = { "connect;mike;mike" };
    serverObj->ParseTokens(buffer, tokArray);
#endif

    // Print it out
    statusOk = serverObj->StartServer();
    printf("Endless loop, as server is up always");

    while (statusOk) {
        statusOk = serverObj->ListenForClient();
        if (statusOk) {
            cout << ">> Connected to client successfully." << endl;
        }
        statusOk = serverObj->rpcProcess(); // Launch thread
        if (statusOk) {
            cout << ">> Completed RPC, waiting for another RPC." << endl;
        }
    }

    delete serverObj;
    return 0;
}
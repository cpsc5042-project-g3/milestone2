#include <iostream>
#include "Server.h"

using namespace std;

int main(int argc, char const* argv[])
{
    int port = atoi(argv[2]);
    bool statusOk;
    auto* serverObj = new Server(port);

    // Start server
    statusOk = serverObj->StartServer();

    // Begin client interaction
    while (statusOk) {
        statusOk = serverObj->ListenForClient();
        if (statusOk) {
            cout << ">> Client session ended." << endl;
            cout << ">> Waiting for next client." << endl;
        }
    }

    delete serverObj;
    return 0;
}
// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#define PORT 8080


using namespace std;
/*
 * Socket connection: exactly same as the server's socket creation.
 */
int main(int argc, char const *argv[]) {

    // TEST OUTPUT
    cout << "args = " << argc << endl;
    cout << "argv = "<< argv[1] << " " << argv[2] << " " << argv[3]
         << " " <<  argv[4] << endl;


    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char *RPC1 = "rpc=connect";

    // Create connection socket file descriptor.
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    cout << "USER > Default IP: 127.0.0.1 | Default PORT: " << PORT << endl;

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    send(sock , RPC1 , strlen(RPC1) , 0 );


    return 0;
}


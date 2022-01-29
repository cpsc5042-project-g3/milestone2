// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>

#define PORT 8080
int main(int argc, char const *argv[]) {

    // TEST OUTPUT
    std::cout << "args = " << argc << std::endl;
    std::cout << "argv = "<< argv[1] << " " << argv[2] << " " << argv[3]
              << " " <<  argv[4] << std::endl;

    int server_fd;    // listen socket file descriptor
    int new_socket;   // connection socket file descriptor
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";


    // Creating listen socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    std::cout << "SERVER > GOT SOCKET" << std::endl;

    /*
     * This helps in manipulating options for the socket referred by the file descriptor sockfd.
     * This is optional, but it helps in reuse of address and port. Prevents error such as: "address already in use".
     */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    std::cout << "SERVER > ABOUT TO BIND\n" << std::endl;
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // It puts the server socket in a passive mode, where it waits for the client to approach the server
    // to make a connection.
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    std::cout << "SERVER > WAITING" << std::endl;


    // At this point, connection is established between client and server, and they are ready to transfer data.
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    std::cout << "SERVER > ACCEPTED & CONNECTED" << std::endl;

    // WAITING ...
    // Receive RPC
    read( new_socket , buffer, 1024);
    printf("SERVER > %s\n",buffer);



    sleep(10);
    return 0;
}

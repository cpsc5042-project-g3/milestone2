/*
 * CPSC 5042, Computing System Principles II
 * Winter Quarter 2022
 * Group Project, Milestone 1
 * Members: Zi Wang, Andrew Shell, Steph Mills, Leonardo Levy
 */

// Server side of networked Guess Who game

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>

#define PORT 10326          // last digit of our port number altered to match our group number
#define MAX_CONNECTIONS 3   // maximum number of client connections

using namespace std;

/*
 * This function assembles a welcome string to be displayed to the operator of the
 * Guess Who server.  The responsibility for outputting the string to the user is
 * belongs to the caller of this function.
 */
string welcomeMessage() {
    string str = "Welcome to the Guess Who Server\n";

    return str;
}

string shutdownMessage() {
    return "Thanks for playing the CPSC 5042 version of Guess Who!";
}

void importCharacters() {}

void selectTarget() {}

bool clientQuestion() {
    bool result;

    return result;
}

bool clientGuess() {
    bool result;

    return result;
}

void parseMessage() {}

/*
 * This function creates a socket on the server for a client to connect to.  If the socket
 * creation fails, the program is terminated.
 */
int createSocket() {
    int socketID = socket(AF_INET, SOCK_STREAM, 0);
    if (socketID == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return socketID;
}

/*
 * This function configures the socket for address and port reuse.  If the configuration
 * fails, the program is terminated.
 */
void configSocket(int socketID, const void *optval) {
    if (setsockopt(socketID, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
}

/*
 * This function binds the socket to the localhost.  If the operation fails,
 * the programe is terminated.
 */
void bindSocket(int socketID, struct sockaddr_in address) {
    if (bind(socketID, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

/*
 * This function listens for a client connection.  If too many connections are sought
 * the connection is refused and the program terminates.
 */
void listen(int socketID) {
    if (listen(socketID, MAX_CONNECTIONS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

/*
 * This function accepts a client connect to a server socket.  If the acceptance
 * fails, the program is terminated.
 */
int acceptConnection(int socketID, struct sockaddr* addrPtr, int addrLen) {
    int new_socket;
    if ((new_socket = accept(socketID, addrPtr, (socklen_t*) &addrLen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    return new_socket;
}

int main(int argc, char const *argv[]) {
    struct sockaddr_in address;
    int addrLen = sizeof(address);
    int socketID, new_socket;
    int opt = 1;
    char buffer[1024] = {0};
    char* messageToClient = "Hello from the server.";


    // display welcome message to operator of server
    cout << welcomeMessage();


    // create and configure socket
    cout << ">> Creating and configuring socket on Port " << PORT << endl;
    socketID = createSocket();
    configSocket(socketID, &opt);


    // bind socket
    cout << ">> Binding socket." << endl;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bindSocket(socketID, address);


    // listen for client connection
    cout << ">> Listening for client connection." << endl;
    listen(socketID);


    // accept client connection
    new_socket = acceptConnection(socketID, (struct sockaddr *)&address, addrLen);
    cout << ">> Client connection accepted." << endl;


    // read message from client
    read(new_socket, buffer, 1024);
    cout << ">> Message read from client: ";
    printf("%s\n", buffer);


    // write message to client
    cout << ">> Sending message to client: " << messageToClient << endl;
    send(new_socket , messageToClient , strlen(messageToClient) , 0 );
    cout << ">> Message sent successfully." << endl;


    // shutdown
    cout << shutdownMessage() << endl;
}
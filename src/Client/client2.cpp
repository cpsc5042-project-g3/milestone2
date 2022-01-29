/*
 * CPSC 5042, Computing System Principles II
 * Winter Quarter 2022
 * Group Project, Milestone 1
 * Members: Zi Wang, Andrew Shell, Steph Mills, Leonardo Levy
 */

// Client side of networked Guess Who game
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#define PORT 10326

using namespace std;

/*
 * This function returns a string displaying the welcome message
 * for the Guess Who game.
 */
string welcomeMessage() {
    string str = "Welcome to the Guess Who Client";
    return str;
}

/*
 * This function returns a shutdown message for the Guess Who client.
 */
string shutdownMessage() {
    return "Thanks for playing Guess Who!";
}

/*
 * This function displays the instructions on how to play the game.
 */
string showRules() {
    string rules;




    return rules;
}

/*
 * This function displays the current activeList.  Initially, activeList
 * is the same as the masterList as received from the server.  After the
 * player executes a removeCharacter command, the corresponding character
 * is removed from activeList.
 */
string showActiveList() {
    string displayList;




    return displayList;
}

/*
 * This function removes the row corresponding to the specified character
 * from the activeList data structure.  rowNum is the row number to be
 * removed, i.e. 1 = Row 1, 12 = Row 12, etc.  After removing a row, the
 * function automatically calls the showActiveList function.
 */
string removeCharacter(int rowNum) {
    string displayList;

    displayList = showActiveList();
    return displayList;
}

/*
 * This function creates a socket on the client side.  If the socket
 * creation fails, the function returns -1 to the caller.
 */
int createSocket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

int main(int argc, char const *argv[])
{
    cout << welcomeMessage() << endl;

    // initialize variables
    int socketID = 0;
    struct sockaddr_in serv_addr;
    const char* serverIP = "127.0.0.1";
    char* hello = "Hello from client";
    char  buffer[1024] = {0};


    // create socket
    cout << ">> Creating socket" << endl;
    socketID = createSocket();
    if (socketID < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }


    // Not sure what this does, but G4G includes it so it stays
    cout << ">> Doing magic" << endl;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);


    // Convert IPv4 and IPv6 addresses from text to binary form
    cout << ">> Validating IP address: " << serverIP << endl;
    if(inet_pton(AF_INET, serverIP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }


    // attempt to connect to server
    cout << ">> Connecting to server" << endl;
    if (connect(socketID, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }


    // send message to server
    cout << ">> Sending message to server: " << hello << endl;
    send(socketID , hello , strlen(hello) , 0 );
    cout << ">> Message sent successfully." << endl;


    // read message from server
    read(socketID , buffer, 1024);
    cout << ">> Reading message from server: ";
    printf("%s\n",buffer );

    // shutdown
    cout << shutdownMessage();
    return 0;
}
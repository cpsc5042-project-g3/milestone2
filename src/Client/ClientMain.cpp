/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 1
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Date: Feb 2022
 */

#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include "Client.h"

using namespace std;

/*
 * This function generates a welcome message for the user when they first connect.
 */
void welcome() {
    cout << "Welcome to Guess Who Game!" << endl;
}

/*
 * This function generates a goodbye message for the user when they finish using the client.
 */
void goodbye() {
    cout << ">> Thank you for playing the Guess Who Game! Come back again!" << endl;
}

/*
 * This function display an initial message to the user, prompting them to login to the server
 * or disconnect.
 */
void displayMenu1() {
    cout << "Please pick an option from the menu: \n"
         << "\t1. Login \n"
         << "\t2. Disconnect\n";
    cout << "Your choice: ";
}

/*
 * This function display the game menu to the user, prompting them for the allowed game functions.
 */
void displayMenu2(char *myName) {
    cout << "\n" << myName << ", please pick an option from the menu: \n"
         << "\t1. Start a new game \n" // login required if not already logged in
         << "\t2. Query Trait\n"
         << "\t3. Eliminate Person\n"
         << "\t4. Make a Guess\n"
         << "\t5. Log off and disconnect\n";
    cout << "Your choice: ";
}

/*
 * This function displays the remaining characters to be guessed from.
 */
void displayCharacterList(Client *client) {
    set<string> localCopy = client->characterList;
    int size = localCopy.size();

    cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    if (size == 24)
        printf("Make a guess among these 24 characters: \n");
    else
        printf("Here are the %lu remaining character(s):\n", localCopy.size());

    int formatter = 0;    // display a max of 10 names per line
    for (string name:localCopy) {
        cout << name << " ";
        formatter++;
        if (formatter % 10 == 0)
            cout << endl;
    }

    cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}

/*
 * This function displays a list of traits user can query about.
 */
void displayTraitList(Client *client) {
    set<string> localCopy = client->traitList;
    cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    printf("Here are the %lu traits you can query about:\n", localCopy.size());

    int formatter = 0;    // display a max of 10 names per line
    for (string name:localCopy) {
        cout << "\"" << name << "\"" << " ";
        formatter++;
        if (formatter % 5 == 0)
            cout << endl;
    }

    cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
}

/*
 * This function gets the menu option the user has selected.  It is usable with either display menu.
 */
int getMenuPick(int menu) {
    string userPick;
    while (true) {
        cin >> userPick;
        if ((menu == 1 && userPick.find_first_not_of("12") == string::npos) ||
            (menu == 2 && userPick.find_first_not_of("12345") == string::npos))
            return userPick[0] - '0';
        else
            cout << "Please pick again: ";
    }
}

/*
 * This function parses the token received in a message between the server and the client.
 */
void ParseTokens(char *buffer, std::vector<std::string> &a) {
    char *token;
    char *rest = (char *) buffer;

    while ((token = strtok_r(rest, ";", &rest))) {
        printf("%s\n", token);
        a.emplace_back(token);
    }
}

int main(int argc, char const *argv[]) {
    const char *serverIP = argv[1];   // IP address
    const int port = atoi(argv[2]);   // PORT number: has to match server PORT
    auto *client = new Client();
    bool connected;
    bool loggedIn = false;
    int menuPick;
    char *myName;

    welcome();

    // Connect to server
    connected = client->connectServer(serverIP, port);

    // Have user log in or disconnect as they wish
    // Allow user to try again until successful
    while (connected && !loggedIn) {
        displayMenu1();
        menuPick = getMenuPick(1);
        if (menuPick == 1)
            loggedIn = client->logIn();
        else if (menuPick == 2) {
            if (client->disconnectServer())
                connected = false;
        }
    }

    // Save current username for display
    myName = client->userName;

    if (connected) {
        // send RPC: get character name list from server
        // send RPC: get trait name list from server
        if (!client->getCharacterNamesFromServer() || !client->getTraitListFromServer())
            connected = false;
        else
            displayCharacterList(client);
    }

    // Start game (Game logic to follow)
    while (connected) {
        displayMenu2(myName);
        menuPick = getMenuPick(2);
        switch (menuPick) {
            case 1: // Start a new game
                cout << "\n>> A new game is started (Previous data to be erased.)" << endl << endl;
                if (!loggedIn) {
                    cout << ">> Log in needed " << endl;
                    loggedIn = client->logIn();
                }
                break;
            case 2: // Query Trait
                displayTraitList(client);
                client->queryTrait();
                break;
            case 3: // Eliminate Person
                cout << "\nFeature not yet available!" << endl << endl;
                break;
            case 4: // Make a Guess
                cout << "\nFeature not yet available!" << endl << endl;
                break;
            case 5: // Disconnect from server
                if (client->disconnectServer())
                    connected = false;
                break;
            default:
                cout << "The program should never reach here.";
                break;
        }
    }

    goodbye();

    // Cleanup memory
    delete client;
    return 0;
}
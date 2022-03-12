/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 12 Mar 2022
 */

#include <vector>
#include <iostream>
#include <iomanip>
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
         << "\t1. Query Trait\n"
         << "\t2. Eliminate Person\n"
         << "\t3. Make a Guess\n"
         << "\t4. Display leaderboard\n"
         << "\t5. Log off and disconnect\n";
    cout << "Your choice: ";
}

/*
 * This function display the game menu to the user, prompting them for the allowed game functions.
 */
void displayMenu3(char *myName) {
    cout << "\n" << myName << ", please pick an option from the menu: \n"
         << "\t1. Display leaderboard\n"
         << "\t2. Log off and disconnect\n";
    cout << "Your choice: ";
}

/*
 * This function displays the remaining characters to be guessed from.
 */
void displayCharacterList(Client *client) {
    // Write table header
    cout << ">> Here is the working list of characters:" << endl;
    cout << "Row  ";

    for (const string &traitName : client->traitNamesForDisplay) {
        cout << setw(12) << left << traitName;
    }
    cout << endl;

    // Write divider line
    cout << "------------------------------------------------------------------------------"
            "---------------------------------" << endl;

    // Write body of table
    int rowNum = 1;
    for (const string &currCharacter : client->characterNames) {

        // write row number and character name
        cout << setw(5) << left << rowNum;

        // write character trait values
        vector<string> traitValues = client->activeList.find(currCharacter)->second;
        for (auto &currValue : traitValues)
            cout << setw(12) << left << currValue;

        // write end of line and prepare for next iteration
        cout << endl;
        rowNum++;
    }
}

/*
 * This function displays a list of traits user can query about.
 */
void displayTraitList(Client *client) {
    cout << "\nHere are the traits you can query about: ";
    cout << "\n-----------------------------------------------------------------------------------" << endl;
    for (const string &name: client->traitNamesForDisplay)
        if (name != "Name")
            cout << "\"" << name << "\"" << " ";
    cout << "\n-----------------------------------------------------------------------------------" << endl;
}

/*
 * This function displays the leader boarder to user.
 */
void printLeaderBoard(const Client &client) {
    cout << "\nCurrent Leaderboard" << endl;
    cout << "\tName        Queries" << endl;
    cout << "\t-------------------" << endl;

    vector<string> leaderBoard = client.leaderBoard;

    // print players in leaderboard
    if (leaderBoard.empty()) {
        cout << "\tLeader board is currently empty." << endl;
        return;
    }

    for (int i = 0; i < leaderBoard.size(); i++) {
        cout << "\t" << setw(12) << left << leaderBoard.at(i) << setw(12)
             << left << stoi(leaderBoard.at(++i)) << endl;
    }
}

/*
 * This function gets the menu option the user has selected.  It is usable with either display menu.
 */
int getMenuPick(int menu) {
    string userPick;
    while (true) {
        cin >> userPick;
        if ((menu == 1 && userPick.find_first_not_of("12") == string::npos) ||
            (menu == 2 && userPick.find_first_not_of("12345") == string::npos) ||
            (menu == 3 && userPick.find_first_not_of("12") == string::npos))
            return userPick[0] - '0';
        else
            cout << "Please pick again: ";
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
        if (menuPick == 1) // Menu 1: login
            loggedIn = client->logIn();
        else if (menuPick == 2) { // Menu 1: disconnect
            if (client->disconnectServer())
                connected = false;
        }
    }

    // Save current username for display
    myName = client->userName;

    if (connected) {
        // send RPC: get character names from server
        // send RPC: get trait names from server
        // send RPC: get trait values from server
        if (!client->getCharacterNamesFromServer() || !client->getTraitNamesFromServer()
            || !client->getTraitValuesFromServer())
            connected = false;
        else
            displayCharacterList(client);
    }

    // Start game (Game logic to follow)
    while (connected) {
        displayMenu2(myName);
        menuPick = getMenuPick(2);
        switch (menuPick) {
            case 1: // Menu 2: Query Trait
                displayTraitList(client);
                client->queryTrait();
                break;
            case 2: // Menu 2: Eliminate Person
                if (client->eliminatePerson())
                    displayCharacterList(client);
                break;
            case 3: // Menu 2: Make a Guess
                client->guessName();

                // After final guess is made and result displayed, display Menu 3
                do {
                    displayMenu3(myName);
                    menuPick = getMenuPick(3);
                    if (menuPick == 1) {  // Menu 3: display leader board
                        client->getLeaderBoard();
                        printLeaderBoard(*client);
                    } else if (menuPick == 2) {  // Menu 3: disconnect
                        if (client->disconnectServer())
                            connected = false;
                        break;
                    }
                } while (menuPick == 1);
                break;
            case 4: // Menu 2: Display leader board
                client->getLeaderBoard();
                printLeaderBoard(*client);
                break;
            case 5: // Menu 2: Disconnect from server
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
/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 10 Mar 2022
 */

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include "Game.h"

using namespace std;

/*
 * This is the default constructor for a Game object
 */
Game::Game() {
    gameID = 0;
    gameCharacter = new Character();
    sourceList = new vector<Character*>();

    // seed random number generator, to be used in gameID
    srand(time(nullptr));

    // initialize game properties
    setGameID();
    setSourceList();
    setGameCharacter();
}

/*
 * This is the destructor for a Game object
 */
Game::~Game() {
    gameID = 0;
    delete gameCharacter;
    delete sourceList;
}

/*
 * This function generates a nonzero random number between 1 and MAX_GAMES
 * and assigns it to the game ID.
 */
void Game::setGameID() {
    // generate gameID
    gameID = rand() % MAX_GAMES + 1;
}

/*
 * This function reads the file of game Characters and their associated
 * traits and stores it in the sourceList data structure.  This function
 * returns false if the operation failed.
 */
void Game::setSourceList() {
    char* rest;
    int lineNum;
    string line;
    vector<string> traitNames, traitValues;

    // open file of characters and their traits
    ifstream inFile("CharacterList.csv");
    if (!inFile.is_open()) {
        cout << ">> Failed to import character properties." << endl;
    }

    // read first line of file to get trait names
    getline(inFile,line);
    lineNum++;

    // parse first line to store character traits in the traitNames vector
    rest = strcpy(new char[line.length() + 1], line.c_str());
    parseTokens(rest, traitNames);

    // read trait values and assign to sourceList
    while (inFile.peek() != EOF) {
        // clear trait values before reading next line
        traitValues.clear();
        // read and parse line
        getline(inFile, line);
        lineNum++;
        rest = strcpy(new char[line.length() + 1], line.c_str());
        parseTokens(rest, traitValues);

        // confirm character data is fully present and no extra info is there.
        // Assumes "Name" is the first column.
        if (traitNames.size() == traitValues.size()) {
            auto *c = new Character();
            for (int i = 0; i < traitNames.size(); i++) {
                if (i == 0) {
                    // assign name
                    c->setName(traitValues.at(i));
                } else {
                    // add trait
                    c->addTrait(traitNames.at(i), traitValues.at(i));
                }
            }
            // Character creation complete.  Add to sourceList.
            if(!addCharacter(c)) {
                cout << ">> Error: Failed to add character on line " << lineNum << endl;
            }
        }
        else {
            // traits are either missing or there is extra information
            cerr << ">> Issue with character specified on line: " << lineNum;
            cerr << ".  Character input skipped." << endl;
        }
    }

    // cleanup
    delete[] rest;
}

/*
 * This function adds a character from the master character list to the
 * sourceList.
 */
bool Game::addCharacter(Character * const characterToAdd) {
    int size = sourceList->size();
    sourceList->push_back(characterToAdd);

    return sourceList->size() > size;
}

/*
 * This function selects a character at random from the sourceList data
 * structure.  It returns false if the data structure is empty or the
 * operation otherwise fails.
 */
void Game::setGameCharacter() {
    // get selected character
    int index = rand() % sourceList->size();
    gameCharacter = sourceList->at(index);
}

/*
 * This function returns the gameID.
 */
int Game::getGameID() const {
    return gameID;
}

/*
 * This function returns the character selected for this game.
 */
Character* Game::getGameCharacter() {
    return gameCharacter;
}

/*
 * This function returns a pointer to the sourceList data structure.  If
 * the operation fails, it returns a nullptr
 */
vector<Character*>* Game::getSourceList() {
    return sourceList;
}

/*
 * This function parses the comma-separated character traits in the
 * "CharacterTraits.txt" file and stores them in the provided vector.
 */
void Game::parseTokens(char* text, vector<string> &v) {
    char* trait;
    char* rest = (char*) text;

    while ((trait = strtok_r(rest, ",", &rest))) {
        v.emplace_back(trait);
    }
}
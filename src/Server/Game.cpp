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
#include <unordered_map>

using namespace std;

const string ALPHABET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const string ENDOFLINE = "\r\n";

/*
 * This is the default constructor for a Game object
 */
Game::Game() {
    gameID = 0;
    gameCharacter = new Character();
    sourceList = new unordered_map<string, Character*>();

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
    int lineNum = 0;
    string line;
    vector<string> traitNames, traitValues;

    // open file of characters and their traits
    ifstream inFile("CharacterList.txt");
    if (!inFile.is_open()) {
        cout << ">> Failed to import character properties." << endl;
    }

    // read first line of file to get trait names
    getline(inFile,line);

    // parse first line to store character traits in the traitNames vector
    rest = strcpy(new char[line.length() + 1], line.c_str());
    parseTokens(rest, traitNames);

    // Convert first line of trait names into a string to pass to Client
    if (lineNum == 0) {
        stringstream ss;
        for (string &traitName: traitNames)
            ss << traitName << ";";
        traitNamesForDisplay = ss.str();
    }


    // read trait values and assign to sourceList
    while (inFile.peek() != EOF) {
        // clear trait values before reading next line
        traitValues.clear();
        // read and parse line
        getline(inFile, line);

        // trim line
        int first = line.find_first_of(ALPHABET);
        int last = line.find_first_of(ENDOFLINE);
        line = line.substr(first, last - first);

        rest = strcpy(new char[line.length() + 1], line.c_str());
        parseTokens(rest, traitValues);

        // confirm character data is fully present and no extra info is there.
        // Assumes "Name" is the first column.
        if (traitNames.size() == traitValues.size()) {
            stringstream tempTraitValueHolder;
            auto *c = new Character();
            for (int i = 0; i < traitNames.size(); i++) {
                if (i == 0) {
                    // assign name for character
                    c->setName(traitValues.at(i));
                    characterNames.push_back(traitValues.at(i));
                } else {
                    // add trait for character
                    c->addTrait(traitNames.at(i), traitValues.at(i));
                    // generate a list of trait values IN ORDER to send to Client
                    tempTraitValueHolder << traitValues.at(i) << ";";
                }
            }
            c->traitValuesForDisplay = tempTraitValueHolder.str();

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
        lineNum++;
    }

    // cleanup
    delete[] rest;
}

/*
 * This function adds a character from the master character list to the
 * sourceList.
 */
bool Game::addCharacter(Character * const characterToAdd) {
    unsigned long size = sourceList->size();
    string stringName = characterToAdd->getName();
    sourceList->insert(make_pair(stringName, characterToAdd));

    return sourceList->size() > size;
}

/*
 * This function selects a character at random from the sourceList data
 * structure.  It returns false if the data structure is empty or the
 * operation otherwise fails.
 */
void Game::setGameCharacter() {
    // get selected character
    unsigned long index = rand() % sourceList->size();
    auto it = sourceList->begin();
    for (int i = 0; i < index; i++)
        it++;
    gameCharacter = &(*it->second);

}

/*
 * This function returns the character selected for this game.
 */
Character* Game::getGameCharacter() {
    return gameCharacter;
}

/*
 * This function parses the comma-separated character traits in the
 * "CharacterTraits.txt" file and stores them in the provided vector.
 */
void Game::parseTokens(char* text, vector<string> &v) {
    char* trait;
    char* rest = (char*) text;

    while ((trait = strtok_r(rest, ";", &rest)))
        v.emplace_back(trait);
}
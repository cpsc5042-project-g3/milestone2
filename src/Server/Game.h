/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 12 Mar 2022
 */

#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "Character.h"
#include <unordered_map>
#include <vector>

class Game {
public:
    vector<string> characterNames;                  // a list of character names
    unordered_map<string, Character*> *sourceList;  // a map of characters and their traits
    string traitNamesForDisplay;                    // a list of trait names IN A SPECIFIC ORDER to send to Client

    Game();
    ~Game();
    void setSourceList();
    bool addCharacter( Character* const characterToAdd);
    void setGameCharacter();
    Character* getGameCharacter();

private:
    Character *gameCharacter;           // the secret character to be guessed

    void parseTokens(char* text, vector<string> &v);
};

#endif //SERVER_GAME_H

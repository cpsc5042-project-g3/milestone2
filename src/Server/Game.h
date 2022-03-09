/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 10 Mar 2022
 */

#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "Character.h"
#include <unordered_map>

const int MAX_GAMES = 10000;

class Game {
public:
    vector<string> characterNames;
    unordered_map<string, Character*> *sourceList;
    string traitNamesForDisplay; // a list of trait names IN ORDER to send to Client

    Game();
    ~Game();
    void setSourceList();
    bool addCharacter( Character* const characterToAdd);
    void setGameCharacter();
    Character* getGameCharacter();

private:
    void parseTokens(char* text, vector<string> &v);

    Character *gameCharacter;

};


#endif //SERVER_GAME_H

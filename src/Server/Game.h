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

const int MAX_GAMES = 1000;

class Game {
public:
    Game();
    ~Game();
    void createGameID();
    bool populateMasterList();
    bool selectGameCharacter();

private:
    int gameID;
    Character character;
    Character sourceList[MAX_GAMES];
};


#endif //SERVER_GAME_H

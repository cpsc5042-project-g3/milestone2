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
const int MAX_GAMES = 10000;

class Game {
public:
    Game();
    ~Game();
    void setGameID();
    void setSourceList();
    bool addCharacter(const Character& characterToAdd);
    void setGameCharacter();
    int getGameID() const;
    Character* getGameCharacter();
    vector<Character*> getSourceList();

private:
    void parseTokens(char* text, vector<string> &v);

    int gameID;
    Character gameCharacter;
    vector<Character*> sourceList;
};


#endif //SERVER_GAME_H

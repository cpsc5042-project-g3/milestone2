/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 10 Mar 2022
 */

#ifndef SERVER_CHARACTER_H
#define SERVER_CHARACTER_H

#include <string>
#include <map>

using namespace std;

const int numTraits = 5;

class Character {
public:
    int nameLength;

    Character();
//    explicit Character(string nameIn); // not used yet
    Character(const Character& copyCharacter);
    virtual ~Character();
    bool addTrait(string traitName, string traitValue);
    bool setName(string characterName);
    bool setTrait(string trait, string traitValue);
    void clearTraits();
    string getName();
    string getTraitNames();
    string getTraitValue(string trait);
    Character& operator= (const Character& copyCharacter);

private:
    char* name;
    map<string, string> traits;
};


#endif //SERVER_CHARACTER_H

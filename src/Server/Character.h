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
#include <set>

using namespace std;

const int numTraits = 5;

class Character {
public:
    map<string, string> traits;         // a map of "trait name-trait value" pairs
    string traitValuesForDisplay;       // trait values to display on Client side

    Character();
//    explicit Character(string nameIn); // not used yet
    Character(const Character& copyCharacter);
    virtual ~Character();
    bool addTrait(string traitName, string traitValue);
    bool setName(string characterName);
    void clearTraits();
    string getName();
    string getTraitNames();
    string checkTraitValue(string trait);
    Character& operator= (const Character& copyCharacter);

private:
    char* name;
    int nameLength;
};


#endif //SERVER_CHARACTER_H

/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 12 Mar 2022
 */

#ifndef SERVER_CHARACTER_H
#define SERVER_CHARACTER_H

#include <string>
#include <map>
#include <set>

using namespace std;

class Character {
public:
    map<string, string> traits;         // a map of "trait name-trait value" pairs
    string traitValuesForDisplay;       // trait values to display on Client side

    Character();
    Character(const Character& copyCharacter);
    virtual ~Character();
    bool addTrait(string traitName, string traitValue);
    bool setName(string characterName);
    string getName();
    string checkTraitValue(string trait);

private:
    char* name;             // character's name
    int nameLength;         // length of character's name

    void clearTraits();
    Character& operator= (const Character& copyCharacter);
};


#endif //SERVER_CHARACTER_H

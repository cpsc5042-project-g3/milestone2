/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 10 Mar 2022
 */

#include "Character.h"
#include <iostream>

using namespace std;

/*
 * This is the constructor object for a character in the Guess Who game.
 */
Character::Character() {
    // initialize name, traits, and trait values to known unassigned values
    name = new char[20];
    nameLength = 0;
    this->clearTraits();
}

/*
 * This is an alternate constructor for a character object, using a provided
 * name as an argument.
 */
//Character::Character(string nameIn) { // Not used, commented out for now
//    name = nameIn;
//    traits.clear();
//}

/*
 * This is the copy constructor for a character object.
 */
Character::Character(const Character &copyCharacter) {
    name = copyCharacter.name;
    traits = copyCharacter.traits;      // copy traits map via overloaded assignment operator
}

/*
 * This is the destructor for a character object.
 */
Character::~Character() {
    traits.clear();
    delete name;
}

/*
 * This function adds the supplied trait and value to the character objects
 * trait map.  If the trait already exists, the function returns false.
 */
bool Character::addTrait(string traitName, string traitValue) {
    // check to see if trait exists
    if (traits.find(traitName) != traits.end()) {
        // trait already exists
        return false;
    }

    // add key-value pair to map
    traits.insert(pair<string, string>(traitName, traitValue));
    return true;
}

/*
 * This function sets the name of the character object.  If the supplied name
 * is empty, the function returns false.
 */
bool Character::setName(string characterName) {
    if (characterName.empty()) {
        return false;
    }
    nameLength = characterName.length();
    for (int i = 0; i < characterName.length(); i++) {
        name[i] = characterName[i];
    }
    return true;
}

/*
 * This function returns all the traits names (not values) for the character
 */
string Character::getTraitNames() {
    string traitNames;
    for (map<string, string>::iterator iter = traits.begin(); iter != traits.end(); ++iter) {
        traitNames += iter->first + " ";
    }
    return traitNames;
}

/*
 * This function set the value of the supplied trait.  If the trait is not
 * found, it returns false.
 */
bool Character::setTrait(string trait, string traitValue) {
    if (traits.find(trait) != traits.end()) {
        // trait exists, update value
        traits.find(trait)->second = traitValue;
        return true;
    }
    // if the code gets here, it failed to find the supplied trait
    return false;
}

/*
 * This function converts character's name from a char array to a string
 * and returns the string.
 */
string Character::getName() {
    string stringName = "";
    for (int i = 0; i < nameLength; i++)
        stringName += name[i];
    return stringName;
}

/*
 * This function retrieves the value of the supplied trait.  If the trait
 * is not found, the function returns an empty string.
 */
string Character::getTraitValue(string trait) {
    string result = "";

    // make sure trait exists
    if (traits.find(trait) != traits.end()) {
        // trait found, return value
        result = traits.find(trait)->second;
    }

    return result;
}

/*
 * This is the overload for the assignment operator.
 */
Character& Character::operator=(const Character& copyCharacter) {
    // check for assignment to self
    if (this != &copyCharacter) {
        name = copyCharacter.name;
        traits = copyCharacter.traits;
    }
    return *this;
}

/*
 * This function allows a Character object to clear it own traits map.
 */
void Character::clearTraits() {
    traits.clear();
}
/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 10 Mar 2022
 */

#include "Character.h"
#include <map>

using namespace std;

/*
 * This is the constructor object for a character in the Guess Who game.
 */
Character::Character() {
    // initialize name, traits, and trait values to known unassigned values
    Character("TBD");
}

Character::Character(string nameIn) {
    name = nameIn;
    traits.clear();
}

/*
 * This is the destructor for a character object.
 */
Character::~Character() {
    traits.clear();
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
    traits.insert(traitName, traitValue);
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
    name = characterName;
    return true;
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
 * This function returns the string assigned to this character's name.
 * If the name has not been assigned, it returns the default value from
 * the constructor.
 */
string Character::getName() {
    return name;
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
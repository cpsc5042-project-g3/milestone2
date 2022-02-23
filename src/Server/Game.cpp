/*
 * CPSC 5042: Comp Systems Principles II
 * Client-Server Project: Milestone 2
 * Group 3: Andrew Shell, Steph Mills, Zi Wang, Leonardo Levy
 * Professor: Michael McKee
 * Due: 10 Mar 2022
 */

#include "Game.h"

/*
 * This is a constructor for a Game object
 */
Game::Game() {}

/*
 * This is the destructor for a Game object
 */
Game::~Game() {}

/*
 * This function generates a nonzero random number between 1 and MAX_GAMES
 * and assigns it to the game ID.
 */
void Game::setGameID() {}

/*
 * This function reads the file of game Characters and their
 * associated traits and stores it in the sourceList data structure.
 * this function returns false in the operation failed.
 */
bool Game::setMasterList() {}

/*
 * This function selects a character at random from the sourceList data
 * structure.  It returns false if the data structure is empty or the
 * operation otherwise fails.
 */
bool Game::setGameCharacter() {}

/*
 * This function returns a pointer to the sourceList data structure.  If
 * the operation fails, it returns a nullptr
 */
Character* Game::getSourceList() {}
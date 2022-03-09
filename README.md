**  Milestone 2 TO DO:**
- Because I can't type, I entered 2 out of range row numbers when eliminating a character.  This caused the Client to exit. --> FIXED, PLEASE CHECK


**- Powerpoint presentation can be started!
**
- CMakeFile




**Milestone 2 already done:**
8 Mar 2022 (Andrew) - 
 - Fixed lack of printout of "Sending trait values:..." on Windows. Trimmed "line" after being read from file.  See line 95 in Game.cpp.
 - Fixed leaderboard display on Server and Client.  Title now reads "Queries" instead of "Score" and query numbers are listed accordingly for each player.
 - Removed Philip as hard-coded character server chooses when a game is started.
 - added waits to RPCs
 - added a sleep to each of the RPCs.  Length of sleep is controlled by a constant at the top of the RPCImpl.cpp file.


March 8 second UPDATE: 
- Fixed client number discrepancy between Client and Server
- Added "userName" to server output when fit
- ClientMain >> fixed logic error associated with Menu3 (so that after guessing, user never go back to Menu2)
- HARD CODED "PHILIP" as our guess character, for testing
- Cleaned up server console output
- Deleted RPCImpl::printToken() function as it's no longer necessary



March 8 UPDATE:
- ClientMain >> Eliminated menu option 1(start a new game)
- ClientMain >> added menu option 4: Display leaderboard
- ClientMain >> added a third menu to be displayed after Final guess is made so that user can view leader board or disconnect
- Client.cpp >> Fixed bug so that server response for traitValue query returns correctly
- Client.cpp >> Added escape character 'x' option so user can return to previous menu as desired
- Game.cpp >> added option to hardcode "Game Character" for testing purpose
- PRCImpl.cpp >> changed LEADERBOARD_SIZE from 10 to 5 to make testing easier
- PRCImpl.cpp >> Added a new RPC: rpcGetLeaderBoard() function to have server send leader board info to client (empty board allowed)
- RPCImpl.cpp >> Rename getMinScore() to getMaxQueryCount() and edited logic so "minScore" is updated correctly on the leader board
- RPCImpl.cpp >> Updated printLeaderBoard() function to print score out of 100 instead of queryCount
- RPCImpl.cpp >> Changed if (score < minScore) to if (score <= minScore) so that if new score == minScore, last player's name is replaced with most recent player
- RPCImpl.cpp >> Added selectionSort() to sort leader board based on scores in descending order


PRIOR:
- RPCImpl.cpp >> created updateLeaderboard() to print out the leaderboard on the server side after a client wins a game.  Would be nice to pass
  that to the client, but I don't think we have time.
- Client.cpp >> modified getEliiminateChoice() to sort rowNumbers vector after user input finishes.  Seems to have fixed the bug where inputting
  row numbers out of order hangs the client.
- CharacterList.txt >> modified header road to turn "Hair" in "Facial Hair" to lower case.  Looks like it was overlooked when we reverted to lower
  case titles.  This enables "Facial hair" as a query item.
- RPCImpl.cpp >> removed double period sent to Client when user queries a trait with wrong value.

 - RPCImpl >> added updateLeaderboard() function as well as a couple other supporting functions, was unable to test fully.  Used a semaphore
   like we did in HW3.  I think that's enough, but we can add a mutex if needed.

- Server >> Game class created on Server side 
- Server >> Character class created on Server side 
- added new RPC called ```getCharacterList```
- renamed getCharacterList function
- added RPCImpl::rpcGetTraitList()
- added RPCImpl::getTraitNames()
- added Character::getTraitNames() to support RPCImpl::getTraitNames()
- updated Game::setSourceList() to call Game::addCharacter()
- added RPCImpl::rpcQueryTrait()
- added new RPC called ```getTraitList```


- Finished implementing RPC QueryTrait
  - Implemented Client::queryTrait() function
  - Added Client::getQueryTraitName() function
  - Added Client::getQueryTraitValue() function (Customized user questions based on traitName selected)
  - Added Client::validateUserInput() function
  - Added Client::formatAnswer() function to standardize "queryTrait message" to make this RPC easier to process
  - Added Client::trim() function to trim leading and trailing zeros
  - Edited RPCImpl::rpcQueryTrait() function to be responsible for parsing "queryTrait" message
  - Moved query checking to new function RPCImpl::queryTraitResponse()
  - Added customized server response in RPCImpl::customizedReply() function
  

- Server: Cleanup up console output for server to group RPCs into logical text blocks in display window
- Client: Modified displayCharacterList() in ClientMain to display characters and traits as a table
- Client: Created a 2D vector to hold the trait values for each character.


- Populated active list table on Client side
  - Added Client::getTraitValuesFromServer() function
  - Added RPCImpl::rpcGetTraitValues() function
  - Adjusted data types of a couple of fields in Game class, Character class, and Client.h to facilitate RPC process, key value retrieval, and printing.
- Implemented Eliminate Person 
  - Added Client::eliminatePerson() function to eliminate character rows locally
  - Added Client::getEliminateChoice() function to get user choice, user input validation included
- Implemented RPC-Guess Name 
  - Added Client::guessName() function to send final guess to server and get response
  - Added Client::getUserGuess() function to get user guess
  - Added RPCImpl::rpcFinalGuess() function to process user guess
  

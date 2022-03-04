Milestone 2 already done:
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
- Edited Client::Main to ensure code is running with expected output
- Edited RPCImpl::rpcQueryTrait() function to be responsible for parsing "queryTrait" message
- Moved query checking to new function RPCImpl::queryTraitResponse()
- Added customized server response in RPCImpl::customizedReply() function



Milestone 2 TO DO:

- Eliminate Person -- consider no longer an RPC
  - (New design idea: Server does not need to know who have been eliminated. Client can just update its local copy by removing character names.)
  - Client >> Ask user who they want to eliminate, get input, update local copy


- RPC: Guess Name
  - ...

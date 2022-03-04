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

Milestone 2 TO DO:

- Eliminate Person -- consider no longer an RPC
  - (New design idea: Server does not need to know who have been eliminated. Client can just update its local copy by removing character names.)
  - Client >> Ask user who they want to eliminate, get input, update local copy


- RPC: Query trait
  - Client >> Ask user which trait they want to query 
  - Client >> Implement ```Client::queryTrait()``` function to send RPC message 


- RPC: Guess Name
  - ...

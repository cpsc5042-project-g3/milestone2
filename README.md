Milestone 2 already done:
- Server >> Game class created on Server side 
- Server >> Character class created on Server side 
- New RPC request created: called ```getCharacterList```
  - Andrew: please change the function title ```RPCImpl::getCharacterList()``` to ```RPCImpl::rpcGetCharacterList()``` to match the other RPC function titles such as ```RPCImpl::rpcConnect``` and ```RPCImpl:Disconnect```


Milestone 2 TO DO:

- Consider add New PRC: called ```getTraitNames```
  - (Goal is to have server send client a list/map of "trait names and trait values" users can query about. This list is only requested once and will be kept as a local copy, similar to the ```getCharacterList``` RPC idea)
  - Client >> Add ```Client::getTraitListFromServer()``` function to send RPC message 
  - Server >> Add function ```RPCImpl::rpcGetTraitList()``` to receive RPC  
  - Server >> Send client a list of "trait names and values" user can query about 
  - Client >> After client receives the list, make a local copy and display the content to standard output 

- Eliminate Person -- consider no longer an PRC
  - (New design idea: Server does not need to know who have been eliminated. Client can just update its local copy by removing character names.)
  - Client >> Ask user who they want to eliminate, get input, update local copy


- RPC: Query trait
  - Client >> Ask user which trait they want to query 
  - Client >> Implement ```Client::queryTrait()``` function to send RPC message 
  - Server >> Add function ```RPCImpl::rpcQueryTrait()``` to receive RPC
  - Server >> Send client a yes/no response 


- RPC: Guess Name
  - ...

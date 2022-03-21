# Client-Server-Group-3-Project
The project’s Guess Who game is based on the board game “Guess Who”, in which two players ask each other yes-or-no questions about a selected character. Each player tries to be the first to guess the other’s character correctly. For this project, the player (the client) plays by themselves through the server. First, the server picks a random character from a list of characters from a selected text file, then the client asks the server questions in a specific structure about specific character traits that the person to be guessed has. The client/player can use the server’s answers to eliminate characters from the list, which allows room for human error (for example, the client could accidentally eliminate the actual person to be guessed, making it more fun). After the client picks the character(s) they want to eliminate, the character table is updated locally to show only the remaining characters. The client may guess the selected character at any time. If the guess is correct, the client wins and their name is added to the leader board; if it’s incorrect, the client loses and the game ends for that client.


The program is written in C++. The client and server output, including error messages, are displayed using the command line interface. A total of 8 Remote Procedure Calls (RPCs) were implemented between the client and the server.
- connect (with login info)
- getCharacterNames
- getTraitNames
- getTraitValues
- queryTrait
- finalGuess
- getLeaderBoard
- disconnect 


Our game is a single player game, but the program allows multiple clients to play the game at the same time. Each client is considered as a separate thread and is associated with one Game object. In the console output, clients are differentiated by their login usernames. A list of approved username and password combinations can be found in the "ApprovedUsers.txt" file in the Server folder. 


Our global context is the game's leaderboard which is updated whenever a client makes a correct guess. The score is based on the number of queries the player took to make the right guess. The less number of queries the higher the player ranks. To prevent race conditions when updating, the leader board is protected by Semaphore and Mutex.


In RPCImpl.cpp, sleep() functions are used to simulate delays to all of our RPCs to demonstrate parallel activity when multiple clients are interacting with the server at the same time. The sleep times can be adjusted to control the flow of the game.


Throughout the code, there are supporting methods that enable a smooth run of the program. Other than the initial login, user input is considered case-insensitive for user convenience and will be formatted behind the scenes. If user accidentally picks the wrong menu option, user may return to previous menu by entering the escape character “x”.



## Build
To build, navigate to the Server and Client folder and enter the following commands in the shell:


```cmake -S . -B build```


```cmake --build build```

## Run
To run the Server: ```./build/Server 127.0.0.1 8080```

To run the Client: 
```./build/Client 127.0.0.1 8080```
